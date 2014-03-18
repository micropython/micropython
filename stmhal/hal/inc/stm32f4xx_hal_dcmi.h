/**
  ******************************************************************************
  * @file    stm32f4xx_hal_dcmi.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   Header file of DCMI HAL module.
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
#ifndef __STM32F4xx_HAL_DCMI_H
#define __STM32F4xx_HAL_DCMI_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"


/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup DCMI
  * @{
  */  

/* Exported types ------------------------------------------------------------*/

/** 
  * @brief DCMI Error source  
  */ 
typedef enum
{ 
  DCMI_ERROR_SYNC = 1,     /*!< Synchronisation error */
  DCMI_OVERRUN   = 2,      /*!< DCMI Overrun */

}DCMI_ErrorTypeDef;   

/** 
  * @brief   DCMI Embedded Synchronisation CODE Init structure definition  
  */ 
typedef struct
{
  uint8_t FrameStartCode; /*!< Specifies the code of the frame start delimiter. */
  uint8_t LineStartCode;  /*!< Specifies the code of the line start delimiter.  */
  uint8_t LineEndCode;    /*!< Specifies the code of the line end delimiter.    */
  uint8_t FrameEndCode;   /*!< Specifies the code of the frame end delimiter.   */
  
}DCMI_CodesInitTypeDef;

/** 
  * @brief   DCMI Init structure definition  
  */ 
typedef struct
{
  uint32_t  SynchroMode;                /*!< Specifies the Synchronization Mode: Hardware or Embedded.
                                             This parameter can be a value of @ref DCMI_Synchronization_Mode */

  uint32_t  PCKPolarity;                /*!< Specifies the Pixel clock polarity: Falling or Rising.
                                             This parameter can be a value of @ref DCMI_PIXCK_Polarity       */

  uint32_t  VSPolarity;                 /*!< Specifies the Vertical synchronization polarity: High or Low.
                                             This parameter can be a value of @ref DCMI_VSYNC_Polarity       */

  uint32_t  HSPolarity;                 /*!< Specifies the Horizontal synchronization polarity: High or Low.
                                             This parameter can be a value of @ref DCMI_HSYNC_Polarity       */

  uint32_t  CaptureRate;                /*!< Specifies the frequency of frame capture: All, 1/2 or 1/4.
                                             This parameter can be a value of @ref DCMI_Capture_Rate         */

  uint32_t  ExtendedDataMode;           /*!< Specifies the data width: 8-bit, 10-bit, 12-bit or 14-bit.
                                             This parameter can be a value of @ref DCMI_Extended_Data_Mode   */
  
  DCMI_CodesInitTypeDef SyncroCode;     /*!< Specifies the code of the frame start delimiter.                */
  
  uint32_t JPEGMode;                    /*!< Enable or Disable the JPEG mode.                                
                                             This parameter can be a value of @ref DCMI_MODE_JPEG            */  
  
}DCMI_InitTypeDef;


/** 
  * @brief  HAL DCMI State structures definition  
  */ 
typedef enum
{
  HAL_DCMI_STATE_RESET             = 0x00,  /*!< DCMI not yet initialized or disabled  */
  HAL_DCMI_STATE_READY             = 0x01,  /*!< DCMI initialized and ready for use    */
  HAL_DCMI_STATE_BUSY              = 0x02,  /*!< DCMI internal processing is ongoing   */
  HAL_DCMI_STATE_TIMEOUT           = 0x03,  /*!< DCMI timeout state                    */
  HAL_DCMI_STATE_ERROR             = 0x04   /*!< DCMI error state                      */
                                                                        
}HAL_DCMI_StateTypeDef;

/** 
  * @brief  DCMI handle Structure definition  
  */   
typedef struct
{  
  DCMI_TypeDef                  *Instance;           /*!< DCMI Register base address   */
    
  DCMI_InitTypeDef              Init;                /*!< DCMI parameters              */
  
  HAL_LockTypeDef               Lock;                /*!< DCMI locking object          */
 
  __IO HAL_DCMI_StateTypeDef    State;               /*!< DCMI state                   */
  
  __IO uint32_t                 XferCount;           /*!< DMA transfer counter         */
  
  __IO uint32_t                 XferSize;            /*!< DMA transfer size            */
  
  uint32_t                      XferTransferNumber;  /*!< DMA transfer number          */  

  uint32_t                      pBuffPtr;            /*!< Pointer to DMA output buffer */    
  
  DMA_HandleTypeDef             *DMA_Handle;         /*!< Pointer to the DMA handler   */

  __IO uint32_t                 ErrorCode;           /*!< DCMI Error code              */  
  
}DCMI_HandleTypeDef;    

/* Exported constants --------------------------------------------------------*/

/** @defgroup DCMI_Exported_Constants
  * @{
  */

/** @defgroup DCMI_Error_Code
  * @{
  */
#define HAL_DCMI_ERROR_NONE      ((uint32_t)0x00000000)    /*!< No error              */
#define HAL_DCMI_ERROR_OVF       ((uint32_t)0x00000001)    /*!< Overflow error        */
#define HAL_DCMI_ERROR_SYNC      ((uint32_t)0x00000002)    /*!< Synchronization error */   
#define HAL_DCMI_ERROR_TIMEOUT   ((uint32_t)0x00000020)    /*!< Timeout error         */
/**
  * @}
  */ 

/** @defgroup DCMI_Capture_Mode 
  * @{
  */ 
#define DCMI_MODE_CONTINUOUS           ((uint32_t)0x00000000)  /*!< The received data are transferred continuously 
                                                                    into the destination memory through the DMA             */
#define DCMI_MODE_SNAPSHOT             ((uint32_t)DCMI_CR_CM)  /*!< Once activated, the interface waits for the start of 
                                                                    frame and then transfers a single frame through the DMA */

#define IS_DCMI_CAPTURE_MODE(MODE)(((MODE) == DCMI_MODE_CONTINUOUS) || \
                                   ((MODE) == DCMI_MODE_SNAPSHOT))
/**
  * @}
  */ 


/** @defgroup DCMI_Synchronization_Mode
  * @{
  */ 
#define DCMI_SYNCHRO_HARDWARE        ((uint32_t)0x00000000)   /*!< Hardware synchronization data capture (frame/line start/stop)
                                                                   is synchronized with the HSYNC/VSYNC signals                  */
#define DCMI_SYNCHRO_EMBEDDED        ((uint32_t)DCMI_CR_ESS)  /*!< Embedded synchronization data capture is synchronized with 
                                                                   synchronization codes embedded in the data flow               */
                                                             
#define IS_DCMI_SYNCHRO(MODE)(((MODE) == DCMI_SYNCHRO_HARDWARE) || \
                              ((MODE) == DCMI_SYNCHRO_EMBEDDED))
/**
  * @}
  */ 


/** @defgroup DCMI_PIXCK_Polarity 
  * @{
  */ 
#define DCMI_PCKPOLARITY_FALLING    ((uint32_t)0x00000000)      /*!< Pixel clock active on Falling edge */
#define DCMI_PCKPOLARITY_RISING     ((uint32_t)DCMI_CR_PCKPOL)  /*!< Pixel clock active on Rising edge  */

#define IS_DCMI_PCKPOLARITY(POLARITY)(((POLARITY) == DCMI_PCKPOLARITY_FALLING) || \
                                      ((POLARITY) == DCMI_PCKPOLARITY_RISING))
/**
  * @}
  */ 


/** @defgroup DCMI_VSYNC_Polarity 
  * @{
  */ 
#define DCMI_VSPOLARITY_LOW     ((uint32_t)0x00000000)     /*!< Vertical synchronization active Low  */
#define DCMI_VSPOLARITY_HIGH    ((uint32_t)DCMI_CR_VSPOL)  /*!< Vertical synchronization active High */

#define IS_DCMI_VSPOLARITY(POLARITY)(((POLARITY) == DCMI_VSPOLARITY_LOW) || \
                                     ((POLARITY) == DCMI_VSPOLARITY_HIGH))
/**
  * @}
  */ 


/** @defgroup DCMI_HSYNC_Polarity 
  * @{
  */ 
#define DCMI_HSPOLARITY_LOW     ((uint32_t)0x00000000)     /*!< Horizontal synchronization active Low  */
#define DCMI_HSPOLARITY_HIGH    ((uint32_t)DCMI_CR_HSPOL)  /*!< Horizontal synchronization active High */

#define IS_DCMI_HSPOLARITY(POLARITY)(((POLARITY) == DCMI_HSPOLARITY_LOW) || \
                                     ((POLARITY) == DCMI_HSPOLARITY_HIGH))
/**
  * @}
  */ 

/** @defgroup DCMI_MODE_JPEG 
  * @{
  */ 
#define DCMI_JPEG_DISABLE   ((uint32_t)0x00000000)    /*!< Mode JPEG Disabled  */
#define DCMI_JPEG_ENABLE    ((uint32_t)DCMI_CR_JPEG)  /*!< Mode JPEG Enabled   */

#define IS_DCMI_MODE_JPEG(JPEG_MODE)(((JPEG_MODE) == DCMI_JPEG_DISABLE) || \
                                     ((JPEG_MODE) == DCMI_JPEG_ENABLE))
/**
  * @}
  */ 

/** @defgroup DCMI_Capture_Rate 
  * @{
  */ 
#define DCMI_CR_ALL_FRAME            ((uint32_t)0x00000000)      /*!< All frames are captured        */
#define DCMI_CR_ALTERNATE_2_FRAME    ((uint32_t)DCMI_CR_FCRC_0)  /*!< Every alternate frame captured */
#define DCMI_CR_ALTERNATE_4_FRAME    ((uint32_t)DCMI_CR_FCRC_1)  /*!< One frame in 4 frames captured */

#define IS_DCMI_CAPTURE_RATE(RATE) (((RATE) == DCMI_CR_ALL_FRAME)         || \
                                    ((RATE) == DCMI_CR_ALTERNATE_2_FRAME) || \
                                    ((RATE) == DCMI_CR_ALTERNATE_4_FRAME))
/**
  * @}
  */ 


/** @defgroup DCMI_Extended_Data_Mode 
  * @{
  */ 
#define DCMI_EXTEND_DATA_8B     ((uint32_t)0x00000000)                       /*!< Interface captures 8-bit data on every pixel clock  */
#define DCMI_EXTEND_DATA_10B    ((uint32_t)DCMI_CR_EDM_0)                    /*!< Interface captures 10-bit data on every pixel clock */
#define DCMI_EXTEND_DATA_12B    ((uint32_t)DCMI_CR_EDM_1)                    /*!< Interface captures 12-bit data on every pixel clock */
#define DCMI_EXTEND_DATA_14B    ((uint32_t)(DCMI_CR_EDM_0 | DCMI_CR_EDM_1))  /*!< Interface captures 14-bit data on every pixel clock */

#define IS_DCMI_EXTENDED_DATA(DATA)(((DATA) == DCMI_EXTEND_DATA_8B)  || \
                                    ((DATA) == DCMI_EXTEND_DATA_10B) || \
                                    ((DATA) == DCMI_EXTEND_DATA_12B) || \
                                    ((DATA) == DCMI_EXTEND_DATA_14B))
/**
  * @}
  */ 

/** @defgroup DCMI_Window_Coordinate 
  * @{
  */ 
#define DCMI_WINDOW_COORDINATE    ((uint32_t)0x3FFF)  /*!< Window coordinate */

#define IS_DCMI_WINDOW_COORDINATE(COORDINATE) ((COORDINATE) <= DCMI_WINDOW_COORDINATE)
/**
  * @}
  */

/** @defgroup DCMI_Window_Height 
  * @{
  */ 
#define DCMI_WINDOW_HEIGHT    ((uint32_t)0x1FFF)  /*!< Window Height */

#define IS_DCMI_WINDOW_HEIGHT(HEIGHT) ((HEIGHT) <= DCMI_WINDOW_HEIGHT)
/**
  * @}
  */ 

/** @defgroup DCMI_interrupt_sources 
  * @{
  */ 
#define DCMI_IT_FRAME    ((uint32_t)DCMI_IER_FRAME_IE)
#define DCMI_IT_OVF      ((uint32_t)DCMI_IER_OVF_IE)
#define DCMI_IT_ERR      ((uint32_t)DCMI_IER_ERR_IE)
#define DCMI_IT_VSYNC    ((uint32_t)DCMI_IER_VSYNC_IE)
#define DCMI_IT_LINE     ((uint32_t)DCMI_IER_LINE_IE)

#define IS_DCMI_CONFIG_IT(IT) ((((IT) & (uint16_t)0xFFE0) == 0x0000) && ((IT) != 0x0000))

#define IS_DCMI_GET_IT(IT) (((IT) == DCMI_IT_FRAME) || \
                            ((IT) == DCMI_IT_OVF)   || \
                            ((IT) == DCMI_IT_ERR)   || \
                            ((IT) == DCMI_IT_VSYNC) || \
                            ((IT) == DCMI_IT_LINE))
/**
  * @}
  */ 

/** @defgroup DCMI_Flags 
  * @{
  */ 
/** 
  * @brief   DCMI SR register  
  */ 
#define DCMI_FLAG_HSYNC     ((uint32_t)0x2001)
#define DCMI_FLAG_VSYNC     ((uint32_t)0x2002)
#define DCMI_FLAG_FNE       ((uint32_t)0x2004)
/** 
  * @brief   DCMI RISR register  
  */ 
#define DCMI_FLAG_FRAMERI    ((uint32_t)DCMI_RISR_FRAME_RIS)
#define DCMI_FLAG_OVFRI      ((uint32_t)DCMI_RISR_OVF_RIS)
#define DCMI_FLAG_ERRRI      ((uint32_t)DCMI_RISR_ERR_RIS)
#define DCMI_FLAG_VSYNCRI    ((uint32_t)DCMI_RISR_VSYNC_RIS)
#define DCMI_FLAG_LINERI     ((uint32_t)DCMI_RISR_LINE_RIS)
/** 
  * @brief   DCMI MISR register  
  */ 
#define DCMI_FLAG_FRAMEMI    ((uint32_t)0x1001)
#define DCMI_FLAG_OVFMI      ((uint32_t)0x1002)
#define DCMI_FLAG_ERRMI      ((uint32_t)0x1004)
#define DCMI_FLAG_VSYNCMI    ((uint32_t)0x1008)
#define DCMI_FLAG_LINEMI     ((uint32_t)0x1010)
#define IS_DCMI_GET_FLAG(FLAG) (((FLAG) == DCMI_FLAG_HSYNC)   || \
                                ((FLAG) == DCMI_FLAG_VSYNC)   || \
                                ((FLAG) == DCMI_FLAG_FNE)     || \
                                ((FLAG) == DCMI_FLAG_FRAMERI) || \
                                ((FLAG) == DCMI_FLAG_OVFRI)   || \
                                ((FLAG) == DCMI_FLAG_ERRRI)   || \
                                ((FLAG) == DCMI_FLAG_VSYNCRI) || \
                                ((FLAG) == DCMI_FLAG_LINERI)  || \
                                ((FLAG) == DCMI_FLAG_FRAMEMI) || \
                                ((FLAG) == DCMI_FLAG_OVFMI)   || \
                                ((FLAG) == DCMI_FLAG_ERRMI)   || \
                                ((FLAG) == DCMI_FLAG_VSYNCMI) || \
                                ((FLAG) == DCMI_FLAG_LINEMI))
                                
#define IS_DCMI_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFE0) == 0x0000) && ((FLAG) != 0x0000))
/**
  * @}
  */ 

/**
  * @}
  */
  
/* Exported macro ------------------------------------------------------------*/
/**
  * @brief  Enable the DCMI.
  * @param  __HANDLE__: DCMI handle
  * @retval None
  */
#define __HAL_DCMI_ENABLE(__HANDLE__)    ((__HANDLE__)->Instance->CR |= DCMI_CR_ENABLE)

/**
  * @brief  Disable the DCMI.
  * @param  __HANDLE__: DCMI handle
  * @retval None
  */
#define __HAL_DCMI_DISABLE(__HANDLE__)   ((__HANDLE__)->Instance->CR &= ~(DCMI_CR_ENABLE))

/* Interrupt & Flag management */
/**
  * @brief  Get the DCMI pending flags.
  * @param  __HANDLE__: DCMI handle
  * @param  __FLAG__: Get the specified flag.
  *         This parameter can be any combination of the following values:
  *            @arg DCMI_FLAG_FRAMERI: Frame capture complete flag mask
  *            @arg DCMI_FLAG_OVFRI: Overflow flag mask
  *            @arg DCMI_FLAG_ERRRI: Synchronization error flag mask
  *            @arg DCMI_FLAG_VSYNCRI: VSYNC flag mask
  *            @arg DCMI_FLAG_LINERI: Line flag mask
  * @retval The state of FLAG.
  */
#define __HAL_DCMI_GET_FLAG(__HANDLE__, __FLAG__)\
((((__FLAG__) & 0x3000) == 0x0)? ((__HANDLE__)->Instance->RISR & (__FLAG__)) :\
 (((__FLAG__) & 0x2000) == 0x0)? ((__HANDLE__)->Instance->MISR & (__FLAG__)) : ((__HANDLE__)->Instance->SR & (__FLAG__)))

/**
  * @brief  Clear the DCMI pending flags.
  * @param  __HANDLE__: DCMI handle
  * @param  __FLAG__: specifies the flag to clear.
  *         This parameter can be any combination of the following values:
  *            @arg DCMI_FLAG_FRAMERI: Frame capture complete flag mask
  *            @arg DCMI_FLAG_OVFRI: Overflow flag mask
  *            @arg DCMI_FLAG_ERRRI: Synchronization error flag mask
  *            @arg DCMI_FLAG_VSYNCRI: VSYNC flag mask
  *            @arg DCMI_FLAG_LINERI: Line flag mask
  * @retval None
  */
#define __HAL_DCMI_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ICR |= (__FLAG__))

/**
  * @brief  Enable the specified DCMI interrupts.
  * @param  __HANDLE__:    DCMI handle
  * @param  __INTERRUPT__: specifies the DCMI interrupt sources to be enabled. 
  *         This parameter can be any combination of the following values:
  *            @arg DCMI_IT_FRAME: Frame capture complete interrupt mask
  *            @arg DCMI_IT_OVF: Overflow interrupt mask
  *            @arg DCMI_IT_ERR: Synchronization error interrupt mask
  *            @arg DCMI_IT_VSYNC: VSYNC interrupt mask
  *            @arg DCMI_IT_LINE: Line interrupt mask
  * @retval None
  */
#define __HAL_DCMI_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER |= (__INTERRUPT__))

/**
  * @brief  Disable the specified DCMI interrupts.
  * @param  __HANDLE__: DCMI handle
  * @param  __INTERRUPT__: specifies the DCMI interrupt sources to be enabled. 
  *         This parameter can be any combination of the following values:
  *            @arg DCMI_IT_FRAME: Frame capture complete interrupt mask
  *            @arg DCMI_IT_OVF: Overflow interrupt mask
  *            @arg DCMI_IT_ERR: Synchronization error interrupt mask
  *            @arg DCMI_IT_VSYNC: VSYNC interrupt mask
  *            @arg DCMI_IT_LINE: Line interrupt mask
  * @retval None
  */
#define __HAL_DCMI_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER &= ~(__INTERRUPT__))

/**
  * @brief  Check whether the specified DCMI interrupt has occurred or not.
  * @param  __HANDLE__: DCMI handle
  * @param  __INTERRUPT__: specifies the DCMI interrupt source to check.
  *         This parameter can be one of the following values:
  *            @arg DCMI_IT_FRAME: Frame capture complete interrupt mask
  *            @arg DCMI_IT_OVF: Overflow interrupt mask
  *            @arg DCMI_IT_ERR: Synchronization error interrupt mask
  *            @arg DCMI_IT_VSYNC: VSYNC interrupt mask
  *            @arg DCMI_IT_LINE: Line interrupt mask
  * @retval The state of INTERRUPT.
  */
#define __HAL_DCMI_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->MISR & (__INTERRUPT__))
    
/* Exported functions --------------------------------------------------------*/  

/* Initialization and de-initialization functions *******************************/
HAL_StatusTypeDef HAL_DCMI_Init(DCMI_HandleTypeDef *hdcmi);
HAL_StatusTypeDef HAL_DCMI_DeInit(DCMI_HandleTypeDef *hdcmi);
void       HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi);
void       HAL_DCMI_MspDeInit(DCMI_HandleTypeDef* hdcmi);

/* IO operation functions *******************************************************/
HAL_StatusTypeDef HAL_DCMI_Start_DMA(DCMI_HandleTypeDef* hdcmi, uint32_t DCMI_Mode, uint32_t pData, uint32_t Length);
HAL_StatusTypeDef HAL_DCMI_Stop(DCMI_HandleTypeDef* hdcmi);
void       HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi);
void       HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi);
void       HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi);
void       HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi);
void              HAL_DCMI_IRQHandler(DCMI_HandleTypeDef *hdcmi);

/* Peripheral Control functions *************************************************/
HAL_StatusTypeDef     HAL_DCMI_ConfigCROP(DCMI_HandleTypeDef *hdcmi, uint32_t X0, uint32_t Y0, uint32_t XSize, uint32_t YSize);
HAL_StatusTypeDef     HAL_DCMI_EnableCROP(DCMI_HandleTypeDef *hdcmi);
HAL_StatusTypeDef     HAL_DCMI_DisableCROP(DCMI_HandleTypeDef *hdcmi);

/* Peripheral State functions ***************************************************/
HAL_DCMI_StateTypeDef HAL_DCMI_GetState(DCMI_HandleTypeDef *hdcmi);
uint32_t              HAL_DCMI_GetError(DCMI_HandleTypeDef *hdcmi);

#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
/**
  * @}
  */ 

/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_DCMI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
