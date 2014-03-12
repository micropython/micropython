/**
  ******************************************************************************
  * @file    stm32f4xx_hal_dma2d.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   Header file of DMA2D HAL module.
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
#ifndef __STM32F4xx_HAL_DMA2D_H
#define __STM32F4xx_HAL_DMA2D_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup DMA2D
  * @{
  */ 
  
/* Exported types ------------------------------------------------------------*/

#define MAX_DMA2D_LAYER  2
   
/** 
  * @brief DMA2D color Structure definition  
  */
typedef struct
{
  uint32_t Blue;               /*!< Configures the blue value.
                                    This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  uint32_t Green;              /*!< Configures the green value. 
                                    This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
            
  uint32_t Red;                /*!< Configures the red value. 
                                    This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
} DMA2D_ColorTypeDef;

/** 
  * @brief DMA2D CLUT Structure definition  
  */
typedef struct
{
  uint32_t *pCLUT;                  /*!< Configures the DMA2D CLUT memory address. */

  uint32_t CLUTColorMode;           /*!< configures the DMA2D CLUT color mode. 
                                         This parameter can be one value of @ref DMA2D_CLUT_CM */
            
  uint32_t Size;                    /*!< configures the DMA2D CLUT size. 
                                         This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
} DMA2D_CLUTCfgTypeDef;

/** 
  * @brief DMA2D Init structure definition  
  */
typedef struct
{
  uint32_t             Mode;               /*!< configures the DMA2D transfer mode.
                                                This parameter can be one value of @ref DMA2D_Mode */
  
  uint32_t             ColorMode;          /*!< configures the color format of the output image.
                                                This parameter can be one value of @ref DMA2D_Color_Mode */

  uint32_t             OutputOffset;       /*!< Specifies the Offset value. 
                                                This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x3FFF. */ 
                                                 
} DMA2D_InitTypeDef;

/** 
  * @brief DMA2D Layer structure definition  
  */
typedef struct
{

  
  uint32_t             InputOffset;       /*!< configures the DMA2D foreground offset.
                                               This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x3FFF. */

  uint32_t             InputColorMode;    /*!< configures the DMA2D foreground color mode . 
                                               This parameter can be one value of @ref DMA2D_Input_Color_Mode */
  
  uint32_t             AlphaMode;         /*!< configures the DMA2D foreground alpha mode. 
                                               This parameter can be one value of @ref DMA2D_ALPHA_MODE */

  uint32_t             InputAlpha;        /*!< Specifies the DMA2D foreground alpha value 
                                               This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
            
} DMA2D_LayerCfgTypeDef;

/** 
  * @brief  HAL DMA2D State structures definition  
  */ 
typedef enum
{
  HAL_DMA2D_STATE_RESET             = 0x00,    /*!< DMA2D not yet initialized or disabled       */
  HAL_DMA2D_STATE_READY             = 0x01,    /*!< Peripheral Initialized and ready for use    */
  HAL_DMA2D_STATE_BUSY              = 0x02,    /*!< an internal process is ongoing              */     
  HAL_DMA2D_STATE_TIMEOUT           = 0x03,    /*!< Timeout state                               */  
  HAL_DMA2D_STATE_ERROR             = 0x04,    /*!< DMA2D state error                           */
  HAL_DMA2D_STATE_SUSPEND           = 0x05     /*!< DMA2D process is suspended                  */
                                                                        
}HAL_DMA2D_StateTypeDef;

/** 
  * @brief  DMA2D handle Structure definition  
  */   
typedef struct __DMA2D_HandleTypeDef
{  
  DMA2D_TypeDef               *Instance;                                                    /*!< DMA2D Register base address       */
  
  DMA2D_InitTypeDef           Init;                                                         /*!< DMA2D communication parameters    */ 
  
  void                        (* XferCpltCallback)(struct __DMA2D_HandleTypeDef * hdma2d);  /*!< DMA2D transfer complete callback  */
  
  void                        (* XferErrorCallback)(struct __DMA2D_HandleTypeDef * hdma2d); /*!< DMA2D transfer error callback     */
  
  DMA2D_LayerCfgTypeDef       LayerCfg[MAX_DMA2D_LAYER];                                    /*!< DMA2D Layers parameters           */  
  
  HAL_LockTypeDef             Lock;                                                         /*!< DMA2D Lock                        */  
  
  __IO HAL_DMA2D_StateTypeDef State;                                                        /*!< DMA2D transfer state              */
  
  __IO uint32_t               ErrorCode;                                                    /*!< DMA2D Error code                  */  
  
} DMA2D_HandleTypeDef;    


/* Exported constants --------------------------------------------------------*/

/** @defgroup DMA2D_Exported_Constants
  * @{
  */  

/** @defgroup DMA2D_Layer 
  * @{
  */
#define IS_DMA2D_LAYER(LAYER) ((LAYER) <= MAX_DMA2D_LAYER)           
/**
  * @}
  */

/** @defgroup DMA2D_Error_Code 
  * @{
  */
#define HAL_DMA2D_ERROR_NONE      ((uint32_t)0x00000000)    /*!< No error             */
#define HAL_DMA2D_ERROR_TE        ((uint32_t)0x00000001)    /*!< Transfer error       */
#define HAL_DMA2D_ERROR_CE        ((uint32_t)0x00000002)    /*!< Configuration error  */   
#define HAL_DMA2D_ERROR_TIMEOUT   ((uint32_t)0x00000020)    /*!< Timeout error        */
/**
  * @}
  */
    
/** @defgroup DMA2D_Mode 
  * @{
  */
#define DMA2D_M2M                            ((uint32_t)0x00000000)             /*!< DMA2D memory to memory transfer mode */
#define DMA2D_M2M_PFC                        ((uint32_t)0x00010000)             /*!< DMA2D memory to memory with pixel format conversion transfer mode */
#define DMA2D_M2M_BLEND                      ((uint32_t)0x00020000)             /*!< DMA2D memory to memory with blending transfer mode */
#define DMA2D_R2M                            ((uint32_t)0x00030000)             /*!< DMA2D register to memory transfer mode */

#define IS_DMA2D_MODE(MODE) (((MODE) == DMA2D_M2M)       || ((MODE) == DMA2D_M2M_PFC) || \
                             ((MODE) == DMA2D_M2M_BLEND) || ((MODE) == DMA2D_R2M))
/**
  * @}
  */  

/** @defgroup DMA2D_Color_Mode 
  * @{
  */
#define DMA2D_ARGB8888                       ((uint32_t)0x00000000)             /*!< ARGB8888 DMA2D color mode */
#define DMA2D_RGB888                         ((uint32_t)0x00000001)             /*!< RGB888 DMA2D color mode   */
#define DMA2D_RGB565                         ((uint32_t)0x00000002)             /*!< RGB565 DMA2D color mode   */
#define DMA2D_ARGB1555                       ((uint32_t)0x00000003)             /*!< ARGB1555 DMA2D color mode */
#define DMA2D_ARGB4444                       ((uint32_t)0x00000004)             /*!< ARGB4444 DMA2D color mode */

#define IS_DMA2D_CMODE(MODE_ARGB) (((MODE_ARGB) == DMA2D_ARGB8888) || ((MODE_ARGB) == DMA2D_RGB888)   || \
                                   ((MODE_ARGB) == DMA2D_RGB565)   || ((MODE_ARGB) == DMA2D_ARGB1555) || \
                                   ((MODE_ARGB) == DMA2D_ARGB4444))
/**
  * @}
  */

/** @defgroup DMA2D_COLOR_VALUE
  * @{
  */

#define COLOR_VALUE             ((uint32_t)0x000000FF)                          /*!< color value mask */

#define IS_DMA2D_ALPHA_VALUE(ALPHA_VALUE) ((ALPHA_VALUE) <= COLOR_VALUE)
#define IS_DMA2D_COLOR(COLOR) ((COLOR) <= COLOR_VALUE)
/**
  * @}
  */    

/** @defgroup DMA2D_SIZE 
  * @{
  */
#define DMA2D_PIXEL          (DMA2D_NLR_PL >> 16)                               /*!< DMA2D pixel per line */
#define DMA2D_LINE           DMA2D_NLR_NL                                       /*!< DMA2D number of line */

#define IS_DMA2D_LINE(LINE)  ((LINE) <= DMA2D_LINE)
#define IS_DMA2D_PIXEL(PIXEL) ((PIXEL) <= DMA2D_PIXEL)
/**
  * @}
  */

/** @defgroup DMA2D_OFFSET 
  * @{
  */
#define DMA2D_OFFSET      DMA2D_FGOR_LO            /*!< Line Offset */

#define IS_DMA2D_OFFSET(OOFFSET) ((OOFFSET) <= DMA2D_OFFSET)
/**
  * @}
  */ 

/** @defgroup DMA2D_Input_Color_Mode
  * @{
  */
#define CM_ARGB8888        ((uint32_t)0x00000000)                               /*!< ARGB8888 color mode */
#define CM_RGB888          ((uint32_t)0x00000001)                               /*!< RGB888 color mode */
#define CM_RGB565          ((uint32_t)0x00000002)                               /*!< RGB565 color mode */
#define CM_ARGB1555        ((uint32_t)0x00000003)                               /*!< ARGB1555 color mode */
#define CM_ARGB4444        ((uint32_t)0x00000004)                               /*!< ARGB4444 color mode */
#define CM_L8              ((uint32_t)0x00000005)                               /*!< L8 color mode */
#define CM_AL44            ((uint32_t)0x00000006)                               /*!< AL44 color mode */
#define CM_AL88            ((uint32_t)0x00000007)                               /*!< AL88 color mode */
#define CM_L4              ((uint32_t)0x00000008)                               /*!< L4 color mode */
#define CM_A8              ((uint32_t)0x00000009)                               /*!< A8 color mode */
#define CM_A4              ((uint32_t)0x0000000A)                               /*!< A4 color mode */

#define IS_DMA2D_INPUT_COLOR_MODE(INPUT_CM) (((INPUT_CM) == CM_ARGB8888) || ((INPUT_CM) == CM_RGB888)   || \
                                             ((INPUT_CM) == CM_RGB565)   || ((INPUT_CM) == CM_ARGB1555) || \
                                             ((INPUT_CM) == CM_ARGB4444) || ((INPUT_CM) == CM_L8)       || \
                                             ((INPUT_CM) == CM_AL44)     || ((INPUT_CM) == CM_AL88)     || \
                                             ((INPUT_CM) == CM_L4)       || ((INPUT_CM) == CM_A8)       || \
                                             ((INPUT_CM) == CM_A4))
/**
  * @}
  */

/** @defgroup DMA2D_ALPHA_MODE
  * @{
  */
#define DMA2D_NO_MODIF_ALPHA       ((uint32_t)0x00000000)  /*!< No modification of the alpha channel value */
#define DMA2D_REPLACE_ALPHA        ((uint32_t)0x00000001)  /*!< Replace original alpha channel value by programmed alpha value */
#define DMA2D_COMBINE_ALPHA        ((uint32_t)0x00000002)  /*!< Replace original alpha channel value by programmed alpha value
                                                                with original alpha channel value                              */

#define IS_DMA2D_ALPHA_MODE(AlphaMode) (((AlphaMode) == DMA2D_NO_MODIF_ALPHA) || \
                                        ((AlphaMode) == DMA2D_REPLACE_ALPHA)  || \
                                        ((AlphaMode) == DMA2D_COMBINE_ALPHA))
/**
  * @}
  */    

/** @defgroup DMA2D_CLUT_CM
  * @{
  */
#define DMA2D_CCM_ARGB8888    ((uint32_t)0x00000000)    /*!< ARGB8888 DMA2D C-LUT color mode */
#define DMA2D_CCM_RGB888      ((uint32_t)0x00000001)    /*!< RGB888 DMA2D C-LUT color mode   */

#define IS_DMA2D_CLUT_CM(CLUT_CM) (((CLUT_CM) == DMA2D_CCM_ARGB8888) || ((CLUT_CM) == DMA2D_CCM_RGB888))
/**
  * @}
  */

/** @defgroup DMA2D_CLUT_SIZE
  * @{
  */
#define DMA2D_CLUT_SIZE    (DMA2D_FGPFCCR_CS >> 8)    /*!< DMA2D C-LUT size */

#define IS_DMA2D_CLUT_SIZE(CLUT_SIZE) ((CLUT_SIZE) <= DMA2D_CLUT_SIZE)
/**
  * @}
  */

/** @defgroup DMA2D_DeadTime 
  * @{
  */
#define LINE_WATERMARK            DMA2D_LWR_LW

#define IS_DMA2D_LineWatermark(LineWatermark) ((LineWatermark) <= LINE_WATERMARK)
/**
  * @}
  */    
    
/** @defgroup DMA2D_Interrupts 
  * @{
  */
#define DMA2D_IT_CE             DMA2D_CR_CEIE    /*!< Configuration Error Interrupt */
#define DMA2D_IT_CTC            DMA2D_CR_CTCIE   /*!< C-LUT Transfer Complete Interrupt */
#define DMA2D_IT_CAE            DMA2D_CR_CAEIE   /*!< C-LUT Access Error Interrupt */
#define DMA2D_IT_TW             DMA2D_CR_TWIE    /*!< Transfer Watermark Interrupt */
#define DMA2D_IT_TC             DMA2D_CR_TCIE    /*!< Transfer Complete Interrupt */
#define DMA2D_IT_TE             DMA2D_CR_TEIE    /*!< Transfer Error Interrupt */

#define IS_DMA2D_IT(IT) (((IT) == DMA2D_IT_CTC) || ((IT) == DMA2D_IT_CAE) || \
                        ((IT) == DMA2D_IT_TW) || ((IT) == DMA2D_IT_TC) || \
                        ((IT) == DMA2D_IT_TE) || ((IT) == DMA2D_IT_CE))
/**
  * @}
  */
      
/** @defgroup DMA2D_Flag 
  * @{
  */
#define DMA2D_FLAG_CE          DMA2D_ISR_CEIF     /*!< Configuration Error Interrupt Flag */
#define DMA2D_FLAG_CTC         DMA2D_ISR_CTCIF    /*!< C-LUT Transfer Complete Interrupt Flag */
#define DMA2D_FLAG_CAE         DMA2D_ISR_CAEIF    /*!< C-LUT Access Error Interrupt Flag */
#define DMA2D_FLAG_TW          DMA2D_ISR_TWIF     /*!< Transfer Watermark Interrupt Flag */
#define DMA2D_FLAG_TC          DMA2D_ISR_TCIF     /*!< Transfer Complete Interrupt Flag */
#define DMA2D_FLAG_TE          DMA2D_ISR_TEIF     /*!< Transfer Error Interrupt Flag */

#define IS_DMA2D_GET_FLAG(FLAG) (((FLAG) == DMA2D_FLAG_CTC) || ((FLAG) == DMA2D_FLAG_CAE) || \
                                ((FLAG) == DMA2D_FLAG_TW)   || ((FLAG) == DMA2D_FLAG_TC)  || \
                                ((FLAG) == DMA2D_FLAG_TE)   || ((FLAG) == DMA2D_FLAG_CE))
/**
  * @}
  */
  
/**
  * @}
  */
/* Exported macro ------------------------------------------------------------*/
/**
  * @brief  Enable the DMA2D.
  * @param  __HANDLE__: DMA2D handle
  * @retval None.
  */
#define __HAL_DMA2D_ENABLE(__HANDLE__)        ((__HANDLE__)->Instance->CR |= DMA2D_CR_START)

/**
  * @brief  Disable the DMA2D.
  * @param  __HANDLE__: DMA2D handle
  * @retval None.
  */
#define __HAL_DMA2D_DISABLE(__HANDLE__)        ((__HANDLE__)->Instance->CR &= ~DMA2D_CR_START)

/* Interrupt & Flag management */
/**
  * @brief  Get the DMA2D pending flags.
  * @param  __HANDLE__: DMA2D handle
  * @param  __FLAG__: Get the specified flag.
  *          This parameter can be any combination of the following values:
  *            @arg DMA2D_FLAG_CE:  Configuration error flag
  *            @arg DMA2D_FLAG_CTC: C-LUT transfer complete flag
  *            @arg DMA2D_FLAG_CAE: C-LUT access error flag
  *            @arg DMA2D_FLAG_TW:  Transfer Watermark flag
  *            @arg DMA2D_FLAG_TC:  Transfer complete flag
  *            @arg DMA2D_FLAG_TE:  Transfer error flag   
  * @retval The state of FLAG.
  */
#define __HAL_DMA2D_GET_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ISR & (__FLAG__))

/**
  * @brief  Clears the DMA2D pending flags.
  * @param  __HANDLE__: DMA2D handle
  * @param  __FLAG__: specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg DMA2D_FLAG_CE:  Configuration error flag
  *            @arg DMA2D_FLAG_CTC: C-LUT transfer complete flag
  *            @arg DMA2D_FLAG_CAE: C-LUT access error flag
  *            @arg DMA2D_FLAG_TW:  Transfer Watermark flag
  *            @arg DMA2D_FLAG_TC:  Transfer complete flag
  *            @arg DMA2D_FLAG_TE:  Transfer error flag    
  * @retval None
  */
#define __HAL_DMA2D_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->IFCR |= (__FLAG__))

/**
  * @brief  Enables the specified DMA2D interrupts.
  * @param  __HANDLE__: DMA2D handle
  * @param __INTERRUPT__: specifies the DMA2D interrupt sources to be enabled. 
  *          This parameter can be any combination of the following values:
  *            @arg DMA2D_IT_CE:  Configuration error interrupt mask
  *            @arg DMA2D_IT_CTC: C-LUT transfer complete interrupt mask
  *            @arg DMA2D_IT_CAE: C-LUT access error interrupt mask
  *            @arg DMA2D_IT_TW:  Transfer Watermark interrupt mask
  *            @arg DMA2D_IT_TC:  Transfer complete interrupt mask
  *            @arg DMA2D_IT_TE:  Transfer error interrupt mask
  * @retval None
  */
#define __HAL_DMA2D_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->CR |= (__INTERRUPT__))

/**
  * @brief  Disables the specified DMA2D interrupts.
  * @param  __HANDLE__: DMA2D handle
  * @param __INTERRUPT__: specifies the DMA2D interrupt sources to be disabled. 
  *          This parameter can be any combination of the following values:
  *            @arg DMA2D_IT_CE:  Configuration error interrupt mask
  *            @arg DMA2D_IT_CTC: C-LUT transfer complete interrupt mask
  *            @arg DMA2D_IT_CAE: C-LUT access error interrupt mask
  *            @arg DMA2D_IT_TW:  Transfer Watermark interrupt mask
  *            @arg DMA2D_IT_TC:  Transfer complete interrupt mask
  *            @arg DMA2D_IT_TE:  Transfer error interrupt mask
  * @retval None
  */
#define __HAL_DMA2D_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->CR &= ~(__INTERRUPT__))

/**
  * @brief  Checks whether the specified DMA2D interrupt has occurred or not.
  * @param  __HANDLE__: DMA2D handle
  * @param  __INTERRUPT__: specifies the DMA2D interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg DMA2D_IT_CE:  Configuration error interrupt mask
  *            @arg DMA2D_IT_CTC: C-LUT transfer complete interrupt mask
  *            @arg DMA2D_IT_CAE: C-LUT access error interrupt mask
  *            @arg DMA2D_IT_TW:  Transfer Watermark interrupt mask
  *            @arg DMA2D_IT_TC:  Transfer complete interrupt mask
  *            @arg DMA2D_IT_TE:  Transfer error interrupt mask
  * @retval The state of INTERRUPT.
  */
#define __HAL_DMA2D_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->CR & (__INTERRUPT__))

/* Exported functions --------------------------------------------------------*/  

/* Initialization and de-initialization functions *******************************/
HAL_StatusTypeDef HAL_DMA2D_Init(DMA2D_HandleTypeDef *hdma2d); 
HAL_StatusTypeDef HAL_DMA2D_DeInit (DMA2D_HandleTypeDef *hdma2d);
void HAL_DMA2D_MspInit(DMA2D_HandleTypeDef* hdma2d);
void HAL_DMA2D_MspDeInit(DMA2D_HandleTypeDef* hdma2d);

/* IO operation functions *******************************************************/
HAL_StatusTypeDef HAL_DMA2D_Start(DMA2D_HandleTypeDef *hdma2d, uint32_t pdata, uint32_t DstAddress, uint32_t Width, uint32_t Heigh);
HAL_StatusTypeDef HAL_DMA2D_BlendingStart(DMA2D_HandleTypeDef *hdma2d, uint32_t SrcAddress1, uint32_t SrcAddress2, uint32_t DstAddress, uint32_t Width,  uint32_t Heigh);
HAL_StatusTypeDef HAL_DMA2D_Start_IT(DMA2D_HandleTypeDef *hdma2d, uint32_t pdata, uint32_t DstAddress, uint32_t Width, uint32_t Heigh);
HAL_StatusTypeDef HAL_DMA2D_BlendingStart_IT(DMA2D_HandleTypeDef *hdma2d, uint32_t SrcAddress1, uint32_t SrcAddress2, uint32_t DstAddress, uint32_t Width, uint32_t Heigh);
HAL_StatusTypeDef HAL_DMA2D_Suspend(DMA2D_HandleTypeDef *hdma2d);
HAL_StatusTypeDef HAL_DMA2D_Resume(DMA2D_HandleTypeDef *hdma2d);
HAL_StatusTypeDef HAL_DMA2D_Abort(DMA2D_HandleTypeDef *hdma2d);
HAL_StatusTypeDef HAL_DMA2D_PollForTransfer(DMA2D_HandleTypeDef *hdma2d, uint32_t Timeout);
void              HAL_DMA2D_IRQHandler(DMA2D_HandleTypeDef *hdma2d);

/* Peripheral Control functions *************************************************/
HAL_StatusTypeDef  HAL_DMA2D_ConfigLayer(DMA2D_HandleTypeDef *hdma2d, uint32_t LayerIdx);
HAL_StatusTypeDef  HAL_DMA2D_ConfigCLUT(DMA2D_HandleTypeDef *hdma2d, DMA2D_CLUTCfgTypeDef CLUTCfg, uint32_t LayerIdx);
HAL_StatusTypeDef  HAL_DMA2D_EnableCLUT(DMA2D_HandleTypeDef *hdma2d, uint32_t LayerIdx);
HAL_StatusTypeDef  HAL_DMA2D_DisableCLUT(DMA2D_HandleTypeDef *hdma2d, uint32_t LayerIdx);
HAL_StatusTypeDef  HAL_DMA2D_ProgramLineEvent(DMA2D_HandleTypeDef *hdma2d, uint32_t Line);

/* Peripheral State functions ***************************************************/
HAL_DMA2D_StateTypeDef HAL_DMA2D_GetState(DMA2D_HandleTypeDef *hdma2d);
uint32_t               HAL_DMA2D_GetError(DMA2D_HandleTypeDef *hdma2d);

#endif /* STM32F427xx || STM32F437xx  || STM32F429xx || STM32F439xx */

/**
  * @}
  */ 

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_DMA2D_H */
 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
