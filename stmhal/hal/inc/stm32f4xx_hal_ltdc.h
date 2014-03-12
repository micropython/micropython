/**
  ******************************************************************************
  * @file    stm32f4xx_hal_ltdc.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   Header file of LTDC HAL module.
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
#ifndef __STM32F4xx_HAL_LTDC_H
#define __STM32F4xx_HAL_LTDC_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32F429xx) || defined(STM32F439xx)
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"


/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup LTDC
  * @{
  */
  
/* Exported types ------------------------------------------------------------*/

#define MAX_LAYER  2
    
/** 
  * @brief  LTDC color structure definition  
  */
typedef struct
{
  uint8_t Blue;                    /*!< Configures the blue value.
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  uint8_t Green;                   /*!< Configures the green value.
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
            
  uint8_t Red;                     /*!< Configures the red value. 
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
                                          
  uint8_t Reserved;                /*!< Reserved 0xFF */                                          
} LTDC_ColorTypeDef;    
    
/** 
  * @brief  LTDC Init structure definition  
  */
typedef struct
{
  uint32_t            HSPolarity;                /*!< configures the horizontal synchronization polarity.
                                                      This parameter can be one value of @ref LTDC_HS_POLARITY */

  uint32_t            VSPolarity;                /*!< configures the vertical synchronization polarity.
                                                      This parameter can be one value of @ref LTDC_VS_POLARITY */

  uint32_t            DEPolarity;                /*!< configures the data enable polarity. 
                                                      This parameter can be one of value of @ref LTDC_DE_POLARITY */

  uint32_t            PCPolarity;                /*!< configures the pixel clock polarity. 
                                                      This parameter can be one of value of @ref LTDC_PC_POLARITY */

  uint32_t            HorizontalSync;            /*!< configures the number of Horizontal synchronization width.
                                                      This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. */ 

  uint32_t            VerticalSync;              /*!< configures the number of Vertical synchronization heigh. 
                                                      This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x7FF. */

  uint32_t            AccumulatedHBP;            /*!< configures the accumulated horizontal back porch width.
                                                      This parameter must be a number between Min_Data = LTDC_HorizontalSync and Max_Data = 0xFFF. */

  uint32_t            AccumulatedVBP;            /*!< configures the accumulated vertical back porch heigh.
                                                      This parameter must be a number between Min_Data = LTDC_VerticalSync and Max_Data = 0x7FF. */  
            
  uint32_t            AccumulatedActiveW;        /*!< configures the accumulated active width. This parameter 
                                                      This parameter must be a number between Min_Data = LTDC_AccumulatedHBP and Max_Data = 0xFFF. */ 

  uint32_t            AccumulatedActiveH;        /*!< configures the accumulated active heigh. This parameter 
                                                      This parameter must be a number between Min_Data = LTDC_AccumulatedVBP and Max_Data = 0x7FF. */

  uint32_t            TotalWidth;                /*!< configures the total width. This parameter 
                                                      This parameter must be a number between Min_Data = LTDC_AccumulatedActiveW and Max_Data = 0xFFF. */

  uint32_t            TotalHeigh;                /*!< configures the total heigh. This parameter 
                                                      This parameter must be a number between Min_Data = LTDC_AccumulatedActiveH and Max_Data = 0x7FF. */
  
  LTDC_ColorTypeDef   Backcolor;                 /*!< Configures the background color. */

} LTDC_InitTypeDef;


/** 
  * @brief  LTDC Layer structure definition  
  */
typedef struct
{
  uint32_t WindowX0;                   /*!< Configures the Window Horizontal Start Position.
                                            This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. */
            
  uint32_t WindowX1;                   /*!< Configures the Window Horizontal Stop Position.
                                            This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. */
  
  uint32_t WindowY0;                   /*!< Configures the Window vertical Start Position.
                                            This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. */

  uint32_t WindowY1;                   /*!< Configures the Window vertical Stop Position.
                                            This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF. */
  
  uint32_t PixelFormat;                /*!< Specifies the pixel format. 
                                            This parameter can be one of value of @ref LTDC_Pixelformat */

  uint32_t Alpha;                      /*!< Specifies the constant alpha used for blending.
                                            This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  uint32_t Alpha0;                     /*!< Configures the default alpha value.
                                            This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  uint32_t BlendingFactor1;            /*!< Select the blending factor 1. 
                                            This parameter can be one of value of @ref LTDC_BlendingFactor1 */

  uint32_t BlendingFactor2;            /*!< Select the blending factor 2. 
                                            This parameter can be one of value of @ref LTDC_BlendingFactor2 */
            
  uint32_t FBStartAdress;              /*!< Configures the color frame buffer address */

  uint32_t ImageWidth;                 /*!< Configures the color frame buffer line length. 
                                            This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x1FFF. */
                                                 
  uint32_t ImageHeight;                /*!< Specifies the number of line in frame buffer. 
                                            This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x7FF. */
  
  LTDC_ColorTypeDef   Backcolor;       /*!< Configures the layer background color. */
  
} LTDC_LayerCfgTypeDef;

/** 
  * @brief  HAL LTDC State structures definition  
  */ 
typedef enum
{
  HAL_LTDC_STATE_RESET             = 0x00,    /*!< LTDC not yet initialized or disabled */  
  HAL_LTDC_STATE_READY             = 0x01,    /*!< LTDC initialized and ready for use   */
  HAL_LTDC_STATE_BUSY              = 0x02,    /*!< LTDC internal process is ongoing     */     
  HAL_LTDC_STATE_TIMEOUT           = 0x03,    /*!< LTDC Timeout state                   */  
  HAL_LTDC_STATE_ERROR             = 0x04     /*!< LTDC state error                     */      
                                                                        
}HAL_LTDC_StateTypeDef;

/** 
  * @brief  LTDC handle Structure definition  
  */   
typedef struct
{  
  LTDC_TypeDef                *Instance;                /*!< LTDC Register base address                */
  
  LTDC_InitTypeDef            Init;                     /*!< LTDC parameters                           */
  
  LTDC_LayerCfgTypeDef        LayerCfg[MAX_LAYER];      /*!< LTDC Layers parameters                    */
  
  HAL_LockTypeDef             Lock;                     /*!< LTDC Lock                                 */  
  
  __IO HAL_LTDC_StateTypeDef  State;                    /*!< LTDC state                                */

  __IO uint32_t               ErrorCode;                /*!< LTDC Error code                           */  
  
} LTDC_HandleTypeDef;    

/* Exported constants --------------------------------------------------------*/
/** @defgroup LTDC_Exported_Constants
  * @{
  */
 
/** @defgroup LTDC_Layer 
  * @{
  */  
#define IS_LTDC_LAYER(LAYER) ((LAYER) <= MAX_LAYER)           
/**
  * @}
  */

/** @defgroup LTDC Error Code 
  * @{
  */  
#define HAL_LTDC_ERROR_NONE      ((uint32_t)0x00000000)    /*!< LTDC No error             */
#define HAL_LTDC_ERROR_TE        ((uint32_t)0x00000001)    /*!< LTDC Transfer error       */
#define HAL_LTDC_ERROR_FU        ((uint32_t)0x00000002)    /*!< LTDC FIFO Underrun        */
#define HAL_LTDC_ERROR_TIMEOUT   ((uint32_t)0x00000020)    /*!< LTDC Timeout error        */

/**
  * @}
  */

/** @defgroup LTDC_HS_POLARITY 
  * @{
  */
#define LTDC_HSPOLARITY_AL                ((uint32_t)0x00000000)                /*!< Horizontal Synchronization is active low. */
#define LTDC_HSPOLARITY_AH                LTDC_GCR_HSPOL                        /*!< Horizontal Synchronization is active high. */

#define IS_LTDC_HSPOL(HSPOL) (((HSPOL) == LTDC_HSPOLARITY_AL) || \
                              ((HSPOL) == LTDC_HSPOLARITY_AH))  
/**
  * @}
  */

/** @defgroup LTDC_VS_POLARITY 
  * @{
  */
#define LTDC_VSPOLARITY_AL                ((uint32_t)0x00000000)                /*!< Vertical Synchronization is active low. */
#define LTDC_VSPOLARITY_AH                LTDC_GCR_VSPOL                        /*!< Vertical Synchronization is active high. */

#define IS_LTDC_VSPOL(VSPOL) (((VSPOL) == LTDC_VSPOLARITY_AL) || \
                              ((VSPOL) == LTDC_VSPOLARITY_AH))
/**
  * @}
  */
  
/** @defgroup LTDC_DE_POLARITY 
  * @{
  */
#define LTDC_DEPOLARITY_AL                ((uint32_t)0x00000000)                /*!< Data Enable, is active low. */
#define LTDC_DEPOLARITY_AH                LTDC_GCR_DEPOL                        /*!< Data Enable, is active high. */

#define IS_LTDC_DEPOL(DEPOL) (((DEPOL) ==  LTDC_DEPOLARITY_AL) || \
                              ((DEPOL) ==  LTDC_DEPOLARITY_AH))
/**
  * @}
  */

/** @defgroup LTDC_PC_POLARITY 
  * @{
  */
#define LTDC_PCPOLARITY_IPC               ((uint32_t)0x00000000)                /*!< input pixel clock. */
#define LTDC_PCPOLARITY_IIPC              LTDC_GCR_PCPOL                        /*!< inverted input pixel clock. */

#define IS_LTDC_PCPOL(PCPOL) (((PCPOL) ==  LTDC_PCPOLARITY_IPC) || \
                              ((PCPOL) ==  LTDC_PCPOLARITY_IIPC))
/**
  * @}
  */

/** @defgroup LTDC_SYNC 
  * @{
  */
#define LTDC_HORIZONTALSYNC               (LTDC_SSCR_HSW >> 16)                 /*!< Horizontal synchronization width. */ 
#define LTDC_VERTICALSYNC                 LTDC_SSCR_VSH                         /*!< Vertical synchronization heigh. */

#define IS_LTDC_HSYNC(HSYNC)   ((HSYNC)  <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_VSYNC(VSYNC)   ((VSYNC)  <= LTDC_VERTICALSYNC)
#define IS_LTDC_AHBP(AHBP)     ((AHBP)   <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_AVBP(AVBP)     ((AVBP)   <= LTDC_VERTICALSYNC)
#define IS_LTDC_AAW(AAW)       ((AAW)    <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_AAH(AAH)       ((AAH)    <= LTDC_VERTICALSYNC)
#define IS_LTDC_TOTALW(TOTALW) ((TOTALW) <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_TOTALH(TOTALH) ((TOTALH) <= LTDC_VERTICALSYNC)
/**
  * @}
  */

/** @defgroup LTDC_BACK_COLOR
  * @{
  */ 
#define LTDC_COLOR                   ((uint32_t)0x000000FF)                     /*!< Color mask */

#define IS_LTDC_BLUEVALUE(BBLUE)    ((BBLUE)  <= LTDC_COLOR)
#define IS_LTDC_GREENVALUE(BGREEN)  ((BGREEN) <= LTDC_COLOR)
#define IS_LTDC_REDVALUE(BRED)      ((BRED)   <= LTDC_COLOR) 
/**
  * @}
  */
      
/** @defgroup LTDC_BlendingFactor1 
  * @{
  */
#define LTDC_BLENDING_FACTOR1_CA                       ((uint32_t)0x00000400)   /*!< Blending factor : Cte Alpha */
#define LTDC_BLENDING_FACTOR1_PAxCA                    ((uint32_t)0x00000600)   /*!< Blending factor : Cte Alpha x Pixel Alpha*/

#define IS_LTDC_BLENDING_FACTOR1(BlendingFactor1) (((BlendingFactor1) == LTDC_BLENDING_FACTOR1_CA) || \
                                                   ((BlendingFactor1) == LTDC_BLENDING_FACTOR1_PAxCA))
/**
  * @}
  */

/** @defgroup LTDC_BlendingFactor2
  * @{
  */
#define LTDC_BLENDING_FACTOR2_CA                       ((uint32_t)0x00000005)   /*!< Blending factor : Cte Alpha */
#define LTDC_BLENDING_FACTOR2_PAxCA                    ((uint32_t)0x00000007)   /*!< Blending factor : Cte Alpha x Pixel Alpha*/

#define IS_LTDC_BLENDING_FACTOR2(BlendingFactor2) (((BlendingFactor2) == LTDC_BLENDING_FACTOR2_CA) || \
                                                   ((BlendingFactor2) == LTDC_BLENDING_FACTOR2_PAxCA))
/**
  * @}
  */
      
/** @defgroup LTDC_Pixelformat 
  * @{
  */
#define LTDC_PIXEL_FORMAT_ARGB8888                  ((uint32_t)0x00000000)      /*!< ARGB8888 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_RGB888                    ((uint32_t)0x00000001)      /*!< RGB888 LTDC pixel format   */
#define LTDC_PIXEL_FORMAT_RGB565                    ((uint32_t)0x00000002)      /*!< RGB565 LTDC pixel format   */        
#define LTDC_PIXEL_FORMAT_ARGB1555                  ((uint32_t)0x00000003)      /*!< ARGB1555 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_ARGB4444                  ((uint32_t)0x00000004)      /*!< ARGB4444 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_L8                        ((uint32_t)0x00000005)      /*!< L8 LTDC pixel format       */
#define LTDC_PIXEL_FORMAT_AL44                      ((uint32_t)0x00000006)      /*!< AL44 LTDC pixel format     */
#define LTDC_PIXEL_FORMAT_AL88                      ((uint32_t)0x00000007)      /*!< AL88 LTDC pixel format     */

#define IS_LTDC_PIXEL_FORMAT(Pixelformat) (((Pixelformat) == LTDC_PIXEL_FORMAT_ARGB8888) || ((Pixelformat) == LTDC_PIXEL_FORMAT_RGB888)   || \
                                           ((Pixelformat) == LTDC_PIXEL_FORMAT_RGB565)   || ((Pixelformat) == LTDC_PIXEL_FORMAT_ARGB1555) || \
                                           ((Pixelformat) == LTDC_PIXEL_FORMAT_ARGB4444) || ((Pixelformat) == LTDC_PIXEL_FORMAT_L8)       || \
                                           ((Pixelformat) == LTDC_PIXEL_FORMAT_AL44)     || ((Pixelformat) == LTDC_PIXEL_FORMAT_AL88))
/**
  * @}
  */

/** @defgroup LTDC_Alpha
  * @{
  */
#define LTDC_ALPHA               LTDC_LxCACR_CONSTA                             /*!< LTDC Cte Alpha mask */

#define IS_LTDC_ALPHA(ALPHA) ((ALPHA) <= LTDC_ALPHA)
/**
  * @}
  */

/** @defgroup LTDC_LAYER_Config
  * @{
  */
#define LTDC_STOPPOSITION                 (LTDC_LxWHPCR_WHSPPOS >> 16)          /*!< LTDC Layer stop position  */
#define LTDC_STARTPOSITION                LTDC_LxWHPCR_WHSTPOS                  /*!< LTDC Layer start position */

#define LTDC_COLOR_FRAME_BUFFER           LTDC_LxCFBLR_CFBLL                    /*!< LTDC Layer Line length    */ 
#define LTDC_LINE_NUMBER                  LTDC_LxCFBLNR_CFBLNBR                 /*!< LTDC Layer Line number    */

#define IS_LTDC_HCONFIGST(HCONFIGST) ((HCONFIGST) <= LTDC_STARTPOSITION)
#define IS_LTDC_HCONFIGSP(HCONFIGSP) ((HCONFIGSP) <= LTDC_STOPPOSITION)
#define IS_LTDC_VCONFIGST(VCONFIGST)  ((VCONFIGST) <= LTDC_STARTPOSITION)
#define IS_LTDC_VCONFIGSP(VCONFIGSP) ((VCONFIGSP) <= LTDC_STOPPOSITION)

#define IS_LTDC_CFBP(CFBP) ((CFBP) <= LTDC_COLOR_FRAME_BUFFER)
#define IS_LTDC_CFBLL(CFBLL) ((CFBLL) <= LTDC_COLOR_FRAME_BUFFER)

#define IS_LTDC_CFBLNBR(CFBLNBR) ((CFBLNBR) <= LTDC_LINE_NUMBER)
/**
  * @}
  */

/** @defgroup LTDC_LIPosition 
  * @{
  */
#define IS_LTDC_LIPOS(LIPOS) ((LIPOS) <= 0x7FF)
/**
  * @}
  */

/** @defgroup LTDC_Interrupts 
  * @{
  */                           
#define LTDC_IT_LI                      LTDC_IER_LIE
#define LTDC_IT_FU                      LTDC_IER_FUIE
#define LTDC_IT_TE                      LTDC_IER_TERRIE
#define LTDC_IT_RR                      LTDC_IER_RRIE

#define IS_LTDC_IT(IT) ((((IT) & (uint32_t)0xFFFFFFF0) == 0x00) && ((IT) != 0x00))
/**
  * @}
  */
      
/** @defgroup LTDC_Flag 
  * @{
  */
#define LTDC_FLAG_LI                     LTDC_ISR_LIF
#define LTDC_FLAG_FU                     LTDC_ISR_FUIF
#define LTDC_FLAG_TE                     LTDC_ISR_TERRIF
#define LTDC_FLAG_RR                     LTDC_ISR_RRIF

#define IS_LTDC_FLAG(FLAG) (((FLAG) == LTDC_FLAG_LI) || ((FLAG) == LTDC_FLAG_FU) || \
                               ((FLAG) == LTDC_FLAG_TERR) || ((FLAG) == LTDC_FLAG_RR))
/**
  * @}
  */    

/**
  * @}
  */  

/* Exported macro ------------------------------------------------------------*/
/**
  * @brief  Enable the LTDC.
  * @param  __HANDLE__: LTDC handle
  * @retval None.
  */
#define __HAL_LTDC_ENABLE(__HANDLE__)    ((__HANDLE__)->Instance->GCR |= LTDC_GCR_LTDCEN)

/**
  * @brief  Disable the LTDC.
  * @param  __HANDLE__: LTDC handle
  * @retval None.
  */
#define __HAL_LTDC_DISABLE(__HANDLE__)   ((__HANDLE__)->Instance->GCR &= ~(LTDC_GCR_LTDCEN))

/**
  * @brief  Enable the LTDC Layer.
  * @param  __HANDLE__: LTDC handle
  * @param  __LAYER__: Specify the layer to be enabled
                       This parameter can be 0 or 1
  * @retval None.
  */
#define __HAL_LTDC_LAYER(__HANDLE__, __LAYER__)   ((LTDC_Layer_TypeDef *)(((uint32_t)((__HANDLE__)->Instance)) + 0x84 + (0x80*(__LAYER__))))

#define __HAL_LTDC_LAYER_ENABLE(__HANDLE__, __LAYER__)  ((__HAL_LTDC_LAYER((__HANDLE__), (__LAYER__)))->CR |= (uint32_t)LTDC_LxCR_LEN)


/**
  * @brief  Disable the LTDC Layer.
  * @param  __HANDLE__: LTDC handle
  * @param  __LAYER__: Specify the layer to be disabled
                       This parameter can be 0 or 1
  * @retval None.
  */
#define __HAL_LTDC_LAYER_DISABLE(__HANDLE__, __LAYER__) ((__HAL_LTDC_LAYER((__HANDLE__), (__LAYER__)))->CR &= ~(uint32_t)LTDC_LxCR_LEN)

/**
  * @brief  Reload  Layer Configuration.
  * @param  __HANDLE__: LTDC handle
  * @retval None.
  */
#define __HAL_LTDC_RELOAD_CONFIG(__HANDLE__)   ((__HANDLE__)->Instance->SRCR |= LTDC_SRCR_IMR)

/* Interrupt & Flag management */
/**
  * @brief  Get the LTDC pending flags.
  * @param  __HANDLE__: LTDC handle
  * @param  __FLAG__: Get the specified flag.
  *          This parameter can be any combination of the following values:
  *            @arg LTDC_FLAG_LI: Line Interrupt flag 
  *            @arg LTDC_FLAG_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_FLAG_TE: Transfer Error interrupt flag
  *            @arg LTDC_FLAG_RR: Register Reload Interrupt Flag 
  * @retval The state of FLAG (SET or RESET).
  */
#define __HAL_LTDC_GET_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ISR & (__FLAG__))

/**
  * @brief  Clears the LTDC pending flags.
  * @param  __HANDLE__: LTDC handle
  * @param  __FLAG__: specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg LTDC_FLAG_LI: Line Interrupt flag 
  *            @arg LTDC_FLAG_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_FLAG_TE: Transfer Error interrupt flag
  *            @arg LTDC_FLAG_RR: Register Reload Interrupt Flag 
  * @retval None
  */
#define __HAL_LTDC_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ICR |= (__FLAG__))

/**
  * @brief  Enables the specified LTDC interrupts.
  * @param  __HANDLE__: LTDC handle
  * @param __INTERRUPT__: specifies the LTDC interrupt sources to be enabled. 
  *          This parameter can be any combination of the following values:
  *            @arg LTDC_IT_LI: Line Interrupt flag 
  *            @arg LTDC_IT_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_IT_TE: Transfer Error interrupt flag
  *            @arg LTDC_IT_RR: Register Reload Interrupt Flag
  * @retval None
  */
#define __HAL_LTDC_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER |= (__INTERRUPT__))

/**
  * @brief  Disables the specified LTDC interrupts.
  * @param  __HANDLE__: LTDC handle
  * @param __INTERRUPT__: specifies the LTDC interrupt sources to be disabled. 
  *          This parameter can be any combination of the following values:
  *            @arg LTDC_IT_LI: Line Interrupt flag 
  *            @arg LTDC_IT_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_IT_TE: Transfer Error interrupt flag
  *            @arg LTDC_IT_RR: Register Reload Interrupt Flag
  * @retval None
  */
#define __HAL_LTDC_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER &= ~(__INTERRUPT__))

/**
  * @brief  Checks whether the specified LTDC interrupt has occurred or not.
  * @param  __HANDLE__: LTDC handle
  * @param  __INTERRUPT__: specifies the LTDC interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg LTDC_IT_LI: Line Interrupt flag 
  *            @arg LTDC_IT_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_IT_TE: Transfer Error interrupt flag
  *            @arg LTDC_IT_RR: Register Reload Interrupt Flag
  * @retval The state of INTERRUPT (SET or RESET).
  */
#define __HAL_LTDC_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->ISR & (__INTERRUPT__))
    
/* Exported functions --------------------------------------------------------*/  

/* Initialization and de-initialization functions *******************************/
HAL_StatusTypeDef HAL_LTDC_Init(LTDC_HandleTypeDef *hltdc);
HAL_StatusTypeDef HAL_LTDC_DeInit(LTDC_HandleTypeDef *hltdc);
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc);
void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* hltdc);
void HAL_LTDC_ErrorCallback(LTDC_HandleTypeDef *hltdc);
void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc);

/* IO operation functions *******************************************************/
void  HAL_LTDC_IRQHandler(LTDC_HandleTypeDef *hltdc);

/* Peripheral Control functions *************************************************/
HAL_StatusTypeDef HAL_LTDC_ConfigLayer(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetWindowSize(LTDC_HandleTypeDef *hltdc, uint32_t XSize, uint32_t YSize, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetWindowPosition(LTDC_HandleTypeDef *hltdc, uint32_t X0, uint32_t Y0, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetPixelFormat(LTDC_HandleTypeDef *hltdc, uint32_t Pixelformat, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetAlpha(LTDC_HandleTypeDef *hltdc, uint32_t Alpha, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetAddress(LTDC_HandleTypeDef *hltdc, uint32_t Address, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_ConfigColorKeying(LTDC_HandleTypeDef *hltdc, uint32_t RGBValue, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_ConfigCLUT(LTDC_HandleTypeDef *hltdc, uint32_t *pCLUT, uint32_t CLUTSize, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_EnableColorKeying(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_DisableColorKeying(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_EnableCLUT(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_DisableCLUT(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_ProgramLineEvent(LTDC_HandleTypeDef *hltdc, uint32_t Line);
HAL_StatusTypeDef HAL_LTDC_EnableDither(LTDC_HandleTypeDef *hltdc);
HAL_StatusTypeDef HAL_LTDC_DisableDither(LTDC_HandleTypeDef *hltdc);

/* Peripheral State functions ***************************************************/
HAL_LTDC_StateTypeDef HAL_LTDC_GetState(LTDC_HandleTypeDef *hltdc);
uint32_t              HAL_LTDC_GetError(LTDC_HandleTypeDef *hltdc);

#endif /* STM32F429xx || STM32F439xx */

/**
  * @}
  */ 

/**
  * @}
  */ 
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_LTDC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
