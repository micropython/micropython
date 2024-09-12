/**
  ******************************************************************************
  * @file    stm32n6xx_hal_ltdc.h
  * @author  MCD Application Team
  * @brief   Header file of LTDC HAL module.
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
#ifndef STM32N6xx_HAL_LTDC_H
#define STM32N6xx_HAL_LTDC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

#if defined (LTDC)

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup LTDC LTDC
  * @brief LTDC HAL module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup LTDC_Exported_Types LTDC Exported Types
  * @{
  */
#define MAX_LAYER  2U

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
                                                      This parameter must be a number between
                                                      Min_Data = 0x000 and Max_Data = 0xFFF. */

  uint32_t            VerticalSync;              /*!< configures the number of Vertical synchronization height.
                                                      This parameter must be a number between
                                                      Min_Data = 0x000 and Max_Data = 0x7FF. */

  uint32_t            AccumulatedHBP;            /*!< configures the accumulated horizontal back porch width.
                                                      This parameter must be a number between
                                                      Min_Data = LTDC_HorizontalSync and Max_Data = 0xFFF. */

  uint32_t            AccumulatedVBP;            /*!< configures the accumulated vertical back porch height.
                                                      This parameter must be a number between
                                                      Min_Data = LTDC_VerticalSync and Max_Data = 0x7FF. */

  uint32_t            AccumulatedActiveW;        /*!< configures the accumulated active width.
                                                      This parameter must be a number between
                                                      Min_Data = LTDC_AccumulatedHBP and Max_Data = 0xFFF. */

  uint32_t            AccumulatedActiveH;        /*!< configures the accumulated active height.
                                                      This parameter must be a number between
                                                      Min_Data = LTDC_AccumulatedVBP and Max_Data = 0x7FF. */

  uint32_t            TotalWidth;                /*!< configures the total width.
                                                      This parameter must be a number between
                                                      Min_Data = LTDC_AccumulatedActiveW and Max_Data = 0xFFF. */

  uint32_t            TotalHeigh;                /*!< configures the total height.
                                                      This parameter must be a number between
                                                      Min_Data = LTDC_AccumulatedActiveH and Max_Data = 0x7FF. */

  LTDC_ColorTypeDef   Backcolor;                 /*!< Configures the background color. */
} LTDC_InitTypeDef;

/**
  * @brief  LTDC Layer structure definition
  */
typedef struct
{
  uint32_t WindowX0;                   /*!< Configures the Window Horizontal Start Position.
                                            This parameter must be a number between
                                            Min_Data = 0x000 and Max_Data = 0xFFF. */

  uint32_t WindowX1;                   /*!< Configures the Window Horizontal Stop Position.
                                            This parameter must be a number between
                                            Min_Data = 0x000 and Max_Data = 0xFFF. */

  uint32_t WindowY0;                   /*!< Configures the Window vertical Start Position.
                                            This parameter must be a number between
                                            Min_Data = 0x000 and Max_Data = 0x7FF. */

  uint32_t WindowY1;                   /*!< Configures the Window vertical Stop Position.
                                            This parameter must be a number between
                                            Min_Data = 0x0000 and Max_Data = 0x7FF. */

  uint32_t PixelFormat;                /*!< Specifies the pixel format.
                                            This parameter can be one of value of @ref LTDC_Pixelformat */

  uint32_t Alpha;                      /*!< Specifies the constant alpha used for blending.
                                            This parameter must be a number between
                                            Min_Data = 0x00 and Max_Data = 0xFF. */

  uint32_t Alpha0;                     /*!< Configures the default alpha value.
                                            This parameter must be a number between
                                            Min_Data = 0x00 and Max_Data = 0xFF. */

  uint32_t BlendingFactor1;            /*!< Select the blending factor 1.
                                            This parameter can be one of value of @ref LTDC_BlendingFactor1 */

  uint32_t BlendingFactor2;            /*!< Select the blending factor 2.
                                            This parameter can be one of value of @ref LTDC_BlendingFactor2 */

  uint32_t FBStartAdress;              /*!< Configures the color frame buffer address */

  uint32_t ImageWidth;                 /*!< Configures the color frame buffer line length.
                                            This parameter must be a number between
                                            Min_Data = 0x0000 and Max_Data = 0x1FFF. */

  uint32_t ImageHeight;                /*!< Specifies the number of line in frame buffer.
                                            This parameter must be a number between
                                            Min_Data = 0x000 and Max_Data = 0x7FF. */

  LTDC_ColorTypeDef   Backcolor;       /*!< Configures the layer background color. */
} LTDC_LayerCfgTypeDef;

/**
  * @brief  LTDC Flexible Layer structure definition
  */
typedef struct
{
  uint32_t WindowX0;                   /*!< Configures the Window Horizontal Start Position.
                                            This parameter must be a number between
                                            Min_Data = 0x000 and Max_Data = 0xFFF. */

  uint32_t WindowX1;                   /*!< Configures the Window Horizontal Stop Position.
                                            This parameter must be a number between
                                            Min_Data = 0x000 and Max_Data = 0xFFF. */

  uint32_t WindowY0;                   /*!< Configures the Window vertical Start Position.
                                            This parameter must be a number between
                                            Min_Data = 0x000 and Max_Data = 0x7FF. */

  uint32_t WindowY1;                   /*!< Configures the Window vertical Stop Position.
                                            This parameter must be a number between
                                            Min_Data = 0x0000 and Max_Data = 0x7FF. */

  uint32_t Alpha;                      /*!< Specifies the constant alpha used for blending.
                                            This parameter must be a number between
                                            Min_Data = 0x00 and Max_Data = 0xFF. */

  uint32_t Alpha0;                     /*!< Configures the default alpha value.
                                            This parameter must be a number between
                                            Min_Data = 0x00 and Max_Data = 0xFF. */

  uint32_t BlendingFactor1;            /*!< Select the blending factor 1.
                                            This parameter can be one of value of @ref LTDC_BlendingFactor1 */

  uint32_t BlendingFactor2;            /*!< Select the blending factor 2.
                                            This parameter can be one of value of @ref LTDC_BlendingFactor2 */

  uint32_t ImageWidth;                 /*!< Configures the color frame buffer line length.
                                            This parameter must be a number between
                                            Min_Data = 0x0000 and Max_Data = 0x1FFF. */

  uint32_t ImageHeight;                /*!< Specifies the number of line in frame buffer.
                                            This parameter must be a number between
                                            Min_Data = 0x000 and Max_Data = 0x7FF. */

  LTDC_ColorTypeDef   Backcolor;       /*!< Configures the layer background color. */
} LTDC_LayerTypeDef;

/**
  * @brief  LTDC Flexible ARGB format structure definition
  */
typedef struct
{
  uint32_t PixelSize;          /*!< Value of ARGB Pixel size. Could be a value of LTDC_ARGB_SIZE
                                  This parameter can be one of value of @ref LTDC_ARGB_SIZE                     */

  uint32_t AlphaPos;           /*!< Position of the Alpha component inside the pixel memory word (in bits)
                                  This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x1F. */

  uint32_t RedPos;             /*!< Position of the Red component inside the pixel memory word (in bits)
                                  This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x1F. */

  uint32_t GreenPos;           /*!< Position of the Green component inside the pixel memory word (in bits)
                                  This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x1F. */

  uint32_t BluePos;            /*!< Position of the Blue component inside the pixel memory word (in bits)
                                  This parameter must be a number between Min_Data = 0x0 and Max_Data = 0xF.   */

  uint32_t AlphaWidth;         /*!< Width of the Alpha component in bits
                                  This parameter must be a number between Min_Data = 0x0 and Max_Data = 0xF.   */

  uint32_t RedWidth;           /*!< Width of the Red component in bits
                                  This parameter must be a number between Min_Data = 0x0 and Max_Data = 0xF.   */

  uint32_t GreenWidth;         /*!< Width of the Green component in bits
                                  This parameter must be a number between Min_Data = 0x0 and Max_Data = 0xF.   */

  uint32_t BlueWidth;          /*!< Width of the Blue component in bits
                                  This parameter must be a number between Min_Data = 0x0 and Max_Data = 0xF.   */
} LTDC_FlexARGBTypeDef;

/**
  * @brief  LTDC Layer Flexible ARGB format structure definition
  */
typedef struct
{
  LTDC_LayerTypeDef Layer;          /*!< Pointer to LTDC_LayerTypeDef structure     */
  LTDC_FlexARGBTypeDef FlexARGB;    /*!< Pointer to LTDC_FlexARGBTypeDef structure  */
  uint32_t ARGBAddress;              /*!< Configures the color frame buffer address  */
} LTDC_LayerFlexARGBTypeDef;

/**
  * @brief  LTDC Flexible YUV format structure definition
  */
typedef struct
{
  uint32_t YUVOrder;             /*!< Y/UV order.
                                      This parameter can be one of value of @ref LTDC_YUV_ORDER        */

  uint32_t LuminanceOrder;       /*!< Y1/Y2 order.
                                      This parameter can be one of value of @ref LTDC_LUMINANCE_ORDER  */

  uint32_t ChrominanceOrder;     /*!< U/V order.
                                      This parameter can be one of value of @ref LTDC_CHROMINANCE_ORDER */

  uint32_t LuminanceRescale;     /*!< Enable or disable luminance rescale.
                                      This parameter can be one of value of @ref LTDC_LUMINANCE_RANGE   */
} LTDC_FlexYUVTypeDef;

/**
  * @brief  LTDC Flexible YUV coplanar structure definition
  */
typedef struct
{
  LTDC_LayerTypeDef Layer;             /*!< Pointer to LTDC_LayerTypeDef structure                           */

  LTDC_FlexYUVTypeDef FlexYUV;         /*!< Pointer to LTDC_FlexYUVTypeDef structure                         */

  uint32_t YUVAddress;                        /*!< YUV Frame Buffer address                                         */

  uint32_t ColorConverter;                    /*!< Configure the YUV to RGB conversion.
                                                   This parameter can be one of value of @ref LTDC_YUV2RGBCONVERTOR */
} LTDC_LayerFlexYUVCoPlanarTypeDef;

/**
  * @brief  LTDC YUV Semiplanar addresses structure definition
  */
typedef struct
{
  uint32_t YAddress;     /*!< Y Frame Buffer address  */
  uint32_t UVAddress;    /*!< UV Frame Buffer address */
} LTDC_YUVSemiPlanarAddressTypeDef;

/**
  * @brief  LTDC YUV Fullplanar addresses structure definition
  */
typedef struct
{
  uint32_t YAddress;    /*!< Y Frame Buffer address */
  uint32_t UAddress;    /*!< U Frame Buffer address */
  uint32_t VAddress;    /*!< V Frame Buffer address */
} LTDC_YUVFullPlanarAddressTypeDef;

/**
  * @brief  LTDC Layer Flexible YUV semi Planar structure definition
  */
typedef struct
{
  LTDC_LayerTypeDef Layer;                                  /*!< Pointer to LTDC_LayerTypeDef              */
  LTDC_FlexYUVTypeDef FlexYUV;                              /*!< Pointer LTDC_FlexYUVTypeDef               */
  LTDC_YUVSemiPlanarAddressTypeDef YUVSemiPlanarAddress;    /*!< Pointer LTDC_YUVSemiPlanarAddressTypeDef  */
  uint32_t ColorConverter;                                    /*!< Configure the YUV to RGB conversion.
                                                                  This parameter can be one of value
                                                                  of @ref LTDC_YUV2RGBCONVERTOR              */
} LTDC_LayerFlexYUVSemiPlanarTypeDef;

/**
  * @brief  LTDC Layer Flexible YUV full Planar structure definition
  */
typedef struct
{
  LTDC_LayerTypeDef Layer;                                  /*!< Pointer to LTDC_LayerTypeDef              */
  LTDC_FlexYUVTypeDef FlexYUV;                              /*!< Pointer LTDC_FlexYUVTypeDef               */
  LTDC_YUVFullPlanarAddressTypeDef YUVFullPlanarAddress;    /*!< Pointer LTDC_YUVFullPlanarAddressTypeDef  */
  uint32_t ColorConverter;                                    /*!< Configure the YUV to RGB conversion.
                                                                   This parameter can be one of value
                                                                   of @ref LTDC_YUV2RGBCONVERTOR              */
} LTDC_LayerFlexYUVFullPlanarTypeDef;

/**
  * @brief  HAL LTDC State structures definition
  */
typedef enum
{
  HAL_LTDC_STATE_RESET             = 0x00U,    /*!< LTDC not yet initialized or disabled */
  HAL_LTDC_STATE_READY             = 0x01U,    /*!< LTDC initialized and ready for use   */
  HAL_LTDC_STATE_BUSY              = 0x02U,    /*!< LTDC internal process is ongoing     */
  HAL_LTDC_STATE_TIMEOUT           = 0x03U,    /*!< LTDC Timeout state                   */
  HAL_LTDC_STATE_ERROR             = 0x04U     /*!< LTDC state error                     */
} HAL_LTDC_StateTypeDef;

/**
  * @brief  LTDC handle Structure definition
  */
#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
typedef struct __LTDC_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */
{
  LTDC_TypeDef                *Instance;                /*!< LTDC Register base address                */

  LTDC_InitTypeDef            Init;                     /*!< LTDC parameters                           */

  LTDC_LayerCfgTypeDef        LayerCfg[MAX_LAYER];      /*!< LTDC Layers parameters                    */

  HAL_LockTypeDef             Lock;                     /*!< LTDC Lock                                 */

  __IO HAL_LTDC_StateTypeDef  State;                    /*!< LTDC state                                */

  __IO uint32_t               ErrorCode;                /*!< LTDC Error code                           */

#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
  void (* LineEventCallback)(struct __LTDC_HandleTypeDef *hltdc);     /*!< LTDC Line Event Callback    */
  void (* ReloadEventCallback)(struct __LTDC_HandleTypeDef *hltdc);   /*!< LTDC Reload Event Callback  */
  void (* ErrorCallback)(struct __LTDC_HandleTypeDef *hltdc);         /*!< LTDC Error Callback         */
  void (* WarningEventCallback)(struct __LTDC_HandleTypeDef *hltdc);  /*!< LTDC Warning Event          */

  void (* MspInitCallback)(struct __LTDC_HandleTypeDef *hltdc);       /*!< LTDC Msp Init callback      */
  void (* MspDeInitCallback)(struct __LTDC_HandleTypeDef *hltdc);     /*!< LTDC Msp DeInit callback    */

#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */


} LTDC_HandleTypeDef;

#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL LTDC Callback ID enumeration definition
  */
typedef enum
{
  HAL_LTDC_MSPINIT_CB_ID            = 0x00U,    /*!< LTDC MspInit callback ID       */
  HAL_LTDC_MSPDEINIT_CB_ID          = 0x01U,    /*!< LTDC MspDeInit callback ID     */

  HAL_LTDC_LINE_EVENT_CB_ID         = 0x02U,    /*!< LTDC Line Event Callback ID    */
  HAL_LTDC_RELOAD_EVENT_CB_ID       = 0x03U,    /*!< LTDC Reload Callback ID        */
  HAL_LTDC_WARNING_EVENT_CB_ID      = 0x08U,    /*!< LTDC Warning Event Callback ID */
  HAL_LTDC_ERROR_CB_ID              = 0x04U     /*!< LTDC Error Callback ID         */

} HAL_LTDC_CallbackIDTypeDef;

/**
  * @brief  HAL LTDC Callback pointer definition
  */
typedef  void (*pLTDC_CallbackTypeDef)(LTDC_HandleTypeDef *hltdc);  /*!< pointer to an LTDC callback function */

#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup LTDC_Exported_Constants LTDC Exported Constants
  * @{
  */

/** @defgroup LTDC_Error_Code LTDC Error Code
  * @{
  */
#define HAL_LTDC_ERROR_NONE               0x00000000U   /*!< LTDC No error             */
#define HAL_LTDC_ERROR_TE                 0x00000001U   /*!< LTDC Transfer error       */
#define HAL_LTDC_ERROR_FU                 0x00000002U   /*!< LTDC FIFO Underrun        */
#define HAL_LTDC_ERROR_CRC                0x00000004U   /*!< LTDC CRC error            */
#define HAL_LTDC_ERROR_TIMEOUT            0x00000020U   /*!< LTDC Timeout error        */
#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
#define  HAL_LTDC_ERROR_INVALID_CALLBACK  0x00000040U   /*!< LTDC Invalid Callback error  */
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup LTDC_Layer LTDC Layer
  * @{
  */
#define LTDC_LAYER_1                      0U       /*!< LTDC Layer 1 */
#define LTDC_LAYER_2                      1U       /*!< LTDC Layer 2 */
/**
  * @}
  */

/** @defgroup LTDC_HS_POLARITY LTDC HS POLARITY
  * @{
  */
#define LTDC_HSPOLARITY_AL                0U                    /*!< Horizontal Synchronization is active low. */
#define LTDC_HSPOLARITY_AH                LTDC_GCR_HSPOL        /*!< Horizontal Synchronization is active high. */
/**
  * @}
  */

/** @defgroup LTDC_VS_POLARITY LTDC VS POLARITY
  * @{
  */
#define LTDC_VSPOLARITY_AL                0U                    /*!< Vertical Synchronization is active low. */
#define LTDC_VSPOLARITY_AH                LTDC_GCR_VSPOL        /*!< Vertical Synchronization is active high. */
/**
  * @}
  */

/** @defgroup LTDC_DE_POLARITY LTDC DE POLARITY
  * @{
  */
#define LTDC_DEPOLARITY_AL                0U                    /*!< Data Enable, is active low. */
#define LTDC_DEPOLARITY_AH                LTDC_GCR_DEPOL        /*!< Data Enable, is active high. */
/**
  * @}
  */

/** @defgroup LTDC_PC_POLARITY LTDC PC POLARITY
  * @{
  */
#define LTDC_PCPOLARITY_IPC               0U                    /*!< input pixel clock. */
#define LTDC_PCPOLARITY_IIPC              LTDC_GCR_PCPOL        /*!< inverted input pixel clock. */
/**
  * @}
  */

/** @defgroup LTDC_SYNC LTDC SYNC
  * @{
  */
#define LTDC_HORIZONTALSYNC               (LTDC_SSCR_HSW >> 16U)    /*!< Horizontal synchronization width. */
#define LTDC_VERTICALSYNC                 LTDC_SSCR_VSH             /*!< Vertical synchronization height. */
/**
  * @}
  */

/** @defgroup LTDC_BACK_COLOR LTDC BACK COLOR
  * @{
  */
#define LTDC_COLOR                        0x000000FFU   /*!< Color mask */
/**
  * @}
  */

/** @defgroup LTDC_BlendingFactor1 LTDC Blending Factor1
  * @{
  */
#define LTDC_BLENDING_FACTOR1_CA          0x00000400U   /*!< Blending factor : Cte Alpha */
#define LTDC_BLENDING_FACTOR1_PAxCA       0x00000600U   /*!< Blending factor : Cte Alpha x Pixel Alpha*/
/**
  * @}
  */

/** @defgroup LTDC_BlendingFactor2 LTDC Blending Factor2
  * @{
  */
#define LTDC_BLENDING_FACTOR2_CA          0x00000005U   /*!< Blending factor : Cte Alpha */
#define LTDC_BLENDING_FACTOR2_PAxCA       0x00000007U   /*!< Blending factor : Cte Alpha x Pixel Alpha*/
/**
  * @}
  */

/** @defgroup LTDC_Pixelformat LTDC Pixel format
  * @{
  */
#define LTDC_PIXEL_FORMAT_ARGB8888     0x0U             /*!< ARGB8888 LTDC pixel format*/
#define LTDC_PIXEL_FORMAT_ABGR8888     0x1U             /*!< ABGR888  LTDC pixel format*/
#define LTDC_PIXEL_FORMAT_RGBA8888     0x2U             /*!< RGBA888  LTDC pixel format*/
#define LTDC_PIXEL_FORMAT_BGRA8888     0x3U             /*!< BGRA8888 LTDC pixel format*/
#define LTDC_PIXEL_FORMAT_RGB565       0x4U             /*!< RGB565   LTDC pixel format*/
#define LTDC_PIXEL_FORMAT_BGR565       0x5U             /*!< BGR565   LTDC pixel format*/
#define LTDC_PIXEL_FORMAT_RGB888       0x6U             /*!< RGB888   LTDC pixel format*/
#define LTDC_PIXEL_FORMAT_ARGB1555     0x7U             /*!< ARGB1555 LTDC pixel format*/
#define LTDC_PIXEL_FORMAT_ARGB4444     0x8U             /*!< ARGB4444 LTDC pixel format*/
#define LTDC_PIXEL_FORMAT_L8           0x9U             /*!< L8 LTDC pixel format      */
#define LTDC_PIXEL_FORMAT_AL44         0xAU             /*!< AL44 LTDC pixel format    */
#define LTDC_PIXEL_FORMAT_AL88         0xBU             /*!< AL88 LTDC pixel format    */
/**
  * @}
  */

/** @defgroup LTDC_Mirror LTDC Mirror Configuration
  * @{
  */
#define LTDC_MIRROR_HORIZONTAL                       0U       /*!< Enable Horizontal Mirroring                  */
#define LTDC_MIRROR_VERTICAL                         1U       /*!< Enable Vertical Mirroring                    */
#define LTDC_MIRROR_HORIZONTAL_VERTICAL              2U       /*!< Enable Horizontal and vertical Mirroring     */
#define LTDC_MIRROR_NONE                             3U       /*!< Disable Horizontal and vertical Mirroring    */
/**
  * @}
  */

/** @defgroup LTDC_ARGB_SIZE LTDC ARGB pixel size in bytes
  * @{
  */
#define LTDC_ARGB_PIXEL_SIZE_1_BYTE      1U       /*!<Custom ARGB with 1 byte per pixel          */
#define LTDC_ARGB_PIXEL_SIZE_2_BYTES     2U       /*!<Custom ARGB with 2 bytes per pixel         */
#define LTDC_ARGB_PIXEL_SIZE_3_BYTES     3U       /*!<Custom ARGB with 3 bytes per pixel         */
#define LTDC_ARGB_PIXEL_SIZE_4_BYTES     4U       /*!<Custom ARGB with 4 bytes per pixel         */
/**
  * @}
  */

/** @defgroup LTDC_YUV_ORDER LTDC YUV order from LSB
  * @{
  */
#define LTDC_YUV_ORDER_LUMINANCE_FIRST   LTDC_LxPCR_YF     /*!<Luminance component first            */
#define LTDC_YUV_ORDER_CHROMINANCE_FIRST 0U                /*!<Chrominance component (U or V) first */
/**
  * @}
  */

/** @defgroup LTDC_CHROMINANCE_ORDER LTDC chrominance order from LSB
  * @{
  */
#define LTDC_YUV_CHROMIANCE_ORDER_U_FIRST   LTDC_LxPCR_CBF    /*!<Chrominance order U first  */
#define LTDC_YUV_CHROMIANCE_ORDER_V_FIRST   0U                /*!<Chrominance order V first  */
/**
  * @}
  */

/** @defgroup LTDC_LUMINANCE_ORDER LTDC luminance order from LSB
  * @{
  */
#define LTDC_YUV_LUMINANCE_ORDER_ODD_FIRST   LTDC_LxPCR_OF     /*!<Lumiance order Y1 then Y2    */
#define LTDC_YUV_LUMINANCE_ORDER_EVEN_FIRST  0U                /*!<Luminance order Y2 then Y1   */
/**
  * @}
  */

/** @defgroup LTDC_LUMINANCE_RANGE LTDC luminance range
  * @{
  */
#define LTDC_YUV_LUMINANCE_RESCALE_ENABLE    LTDC_LxPCR_YREN   /*!< Rescale Luminance from [16-235] to [0-255] */
#define LTDC_YUV_LUMINANCE_RESCALE_DISABLE   0U                /*!< Luminance provided in range [0-255]        */
/**
  * @}
  */

/** @defgroup LTDC_BLENDING_ORDER LTDC layers blending order
  * @{
  */
#define LTDC_BLENDING_ORDER_FOREGROUND    LTDC_LxBFCR_BOR   /*!<Layer set to Foreground during blending */
#define LTDC_BLENDING_ORDER_BACKGROUND    0U                /*!<Layer set to Background during blending  */
/**
  * @}
  */

/** @defgroup LTDC_RGB_COMPONENT LTDC RGB component
  * @{
  */
#define LTDC_RGB_COMPONENT_RED   LTDC_GCCR_REN                                   /*!<Red component                   */
#define LTDC_RGB_COMPONENT_GREEN LTDC_GCCR_GEN                                   /*!<Green component                 */
#define LTDC_RGB_COMPONENT_BLUE  LTDC_GCCR_BEN                                   /*!<Blue component                  */
#define LTDC_RGB_COMPONENT_ALL   (LTDC_GCCR_REN | LTDC_GCCR_GEN | LTDC_GCCR_BEN) /*!<Red, Blue and Green components  */
/**
  * @}
  */

/** @defgroup LTDC_DISPLAY LTDC Display
  * @{
  */
#define LTDC_OUT_RGB       0U                                                       /*!< Select RGB as output format (default)    */
#define LTDC_OUT_YUV_HDTV  (LTDC_EDCR_OCYEN  | LTDC_EDCR_OCYSEL)                    /*!< RGB converted to YUV using ITU-R BT.709  */
#define LTDC_OUT_YUV_SDTV  LTDC_EDCR_OCYEN                                          /*!< RGB converted to YUV using ITU-R BT.601  */
#define LTDC_OUT_YVU_HDTV  (LTDC_EDCR_OCYEN | LTDC_EDCR_OCYSEL | LTDC_EDCR_OCYCO)   /*!< RGB converted to YVU using ITU-R BT.709  */
#define LTDC_OUT_YVU_SDTV  (LTDC_EDCR_OCYEN | LTDC_EDCR_OCYCO)                      /*!< RGB converted to YVU using ITU-R BT.601  */


/**
  * @}
  */
/** @defgroup LTDC_YUV2RGBCONVERTOR LTDC YUV to RGB color convertor
  * @{
 */
#define LTDC_YUV2RGBCONVERTOR_BT601_REDUCED_RANGE  0U                /*!< Select ITU-R BT.601 as output format      */
#define LTDC_YUV2RGBCONVERTOR_BT709_REDUCED_RANGE  1U                /*!< Select ITU-R BT.709 as output format      */
#define LTDC_YUV2RGBCONVERTOR_BT601_FULL_RANGE     2U                /*!< Select RGB as output format               */
#define LTDC_YUV2RGBCONVERTOR_BT709_FULL_RANGE     3U                /*!< Select RGB as output format               */
/**
  * @}
  */

/** @defgroup LTDC_Alpha LTDC Alpha
  * @{
  */
#define LTDC_ALPHA                        LTDC_LxCACR_CONSTA        /*!< LTDC Constant Alpha mask */
/**
  * @}
  */

/** @defgroup LTDC_LAYER_Config LTDC LAYER Config
  * @{
  */
#define LTDC_STOPPOSITION                 (LTDC_LxWHPCR_WHSPPOS >> 16U) /*!< LTDC Layer stop position  */
#define LTDC_STARTPOSITION                LTDC_LxWHPCR_WHSTPOS          /*!< LTDC Layer start position */

#define LTDC_COLOR_FRAME_BUFFER           LTDC_LxCFBLR_CFBLL            /*!< LTDC Layer Line length    */
#define LTDC_LINE_NUMBER                  LTDC_LxCFBLNR_CFBLNBR         /*!< LTDC Layer Line number    */
/**
  * @}
  */

/** @defgroup LTDC_Interrupts LTDC Interrupts
  * @{
  */
#define LTDC_IT_LI                        LTDC_IER_LIE              /*!< LTDC Line Interrupt                  */
#define LTDC_IT_FU                        LTDC_IER_FUIE             /*!< LTDC FIFO Underrun Interrupt         */
#define LTDC_IT_TE                        LTDC_IER_TERRIE           /*!< LTDC Transfer Error Interrupt        */
#define LTDC_IT_RR                        LTDC_IER_RRIE             /*!< LTDC Register Reload Interrupt       */
#define LTDC_IT_CRC                       LTDC_IER_CRCIE            /*!< LTDC CRC error Interrupt             */
#define LTDC_IT_FUW                       LTDC_IER_FUWIE            /*!< LTDC FIFO Warning Underrun Interrupt */
/**
  * @}
  */

/** @defgroup LTDC_Flags LTDC Flags
  * @{
  */
#define LTDC_FLAG_LI                      LTDC_ISR_LIF              /*!< LTDC Line Interrupt Flag             */
#define LTDC_FLAG_FU                      LTDC_ISR_FUIF             /*!< LTDC FIFO Underrun interrupt Flag    */
#define LTDC_FLAG_TE                      LTDC_ISR_TERRIF           /*!< LTDC Transfer Error interrupt Flag   */
#define LTDC_FLAG_RR                      LTDC_ISR_RRIF             /*!< LTDC Register Reload interrupt Flag  */
#define LTDC_FLAG_CRC                     LTDC_ISR_CRCIF            /*!< LTDC CRC error Interrupt             */
#define LTDC_FLAG_FUW                     LTDC_ISR_FUWIF            /*!< LTDC FIFO Warning Underrun Interrupt */
/**
  * @}
  */

/** @defgroup LTDC_Reload_Type LTDC Reload Type
  * @{
  */
#define LTDC_RELOAD_IMMEDIATE             LTDC_SRCR_IMR             /*!< Immediate Reload */
#define LTDC_RELOAD_VERTICAL_BLANKING     LTDC_SRCR_VBR             /*!< Vertical Blanking Reload */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup LTDC_Exported_Macros LTDC Exported Macros
  * @{
  */

/** @brief Reset LTDC handle state.
  * @param  __HANDLE__  LTDC handle
  * @retval None
  */
#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
#define __HAL_LTDC_RESET_HANDLE_STATE(__HANDLE__) do{                                                 \
                                                      (__HANDLE__)->State = HAL_LTDC_STATE_RESET;     \
                                                      (__HANDLE__)->MspInitCallback = NULL;           \
                                                      (__HANDLE__)->MspDeInitCallback = NULL;         \
                                                    } while(0)
#else
#define __HAL_LTDC_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_LTDC_STATE_RESET)
#endif /*USE_HAL_LTDC_REGISTER_CALLBACKS */

/**
  * @brief  Enable the LTDC.
  * @param  __HANDLE__  LTDC handle
  * @retval None.
  */
#define __HAL_LTDC_ENABLE(__HANDLE__)    ((__HANDLE__)->Instance->GCR |= LTDC_GCR_LTDCEN)

/**
  * @brief  Disable the LTDC.
  * @param  __HANDLE__  LTDC handle
  * @retval None.
  */
#define __HAL_LTDC_DISABLE(__HANDLE__)   ((__HANDLE__)->Instance->GCR &= ~(LTDC_GCR_LTDCEN))

/**
  * @brief  Enable the LTDC Layer.
  * @param  __HANDLE__  LTDC handle
  * @param  __LAYER__   Specify the layer to be enabled.
  *                     This parameter can be LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1).
  * @retval None.
  */
#define __HAL_LTDC_LAYER_ENABLE(__HANDLE__, __LAYER__)  ((LTDC_LAYER((__HANDLE__), (__LAYER__)))->CR\
                                                         |= (uint32_t)LTDC_LxCR_LEN)

/**
  * @brief  Disable the LTDC Layer.
  * @param  __HANDLE__  LTDC handle
  * @param  __LAYER__   Specify the layer to be disabled.
  *                     This parameter can be LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1).
  * @retval None.
  */
#define __HAL_LTDC_LAYER_DISABLE(__HANDLE__, __LAYER__) ((LTDC_LAYER((__HANDLE__), (__LAYER__)))->CR\
                                                         &= ~(uint32_t)LTDC_LxCR_LEN)

/**
  * @brief  Reload immediately all LTDC Layers.
  * @param  __HANDLE__  LTDC handle
  * @retval None.
  */
#define __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(__HANDLE__)  ((__HANDLE__)->Instance->SRCR |= LTDC_SRCR_IMR)

/**
  * @brief  Reload during vertical blanking period all LTDC Layers.
  * @param  __HANDLE__  LTDC handle
  * @retval None.
  */
#define __HAL_LTDC_VERTICAL_BLANKING_RELOAD_CONFIG(__HANDLE__)  ((__HANDLE__)->Instance->SRCR |= LTDC_SRCR_VBR)

/* Interrupt & Flag management */
/**
  * @brief  Get the LTDC pending flags.
  * @param  __HANDLE__  LTDC handle
  * @param  __FLAG__    Get the specified flag.
  *          This parameter can be any combination of the following values:
  *            @arg LTDC_FLAG_LI: Line Interrupt flag
  *            @arg LTDC_FLAG_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_FLAG_TE: Transfer Error interrupt flag
  *            @arg LTDC_FLAG_RR: Register Reload Interrupt Flag
  *            @arg LTDC_FLAG_CRC: LTDC CRC error interrupt flag
  *            @arg LTDC_FLAG_FUW: FIFO Warning Underrun interrupt flag
  * @retval The state of FLAG (SET or RESET).
  */
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define __HAL_LTDC_GET_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ISR2 & (__FLAG__))
#else
#define __HAL_LTDC_GET_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ISR & (__FLAG__))
#endif /* __ARM_FEATURE_CMSE & __ARM_FEATURE_CMSE == 3U */

/**
  * @brief  Clears the LTDC pending flags.
  * @param  __HANDLE__  LTDC handle
  * @param  __FLAG__    Specify the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg LTDC_FLAG_LI: Line Interrupt flag
  *            @arg LTDC_FLAG_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_FLAG_TE: Transfer Error interrupt flag
  *            @arg LTDC_FLAG_RR: Register Reload Interrupt Flag
  *            @arg LTDC_FLAG_CRC: LTDC CRC error Interrupt flag
  *            @arg LTDC_FLAG_FUW: FIFO Warning Underrun Interrupt flag
  * @retval None
  */
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define __HAL_LTDC_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ICR2 = (__FLAG__))
#else
#define __HAL_LTDC_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ICR = (__FLAG__))
#endif /* __ARM_FEATURE_CMSE & __ARM_FEATURE_CMSE == 3U */

/**
  * @brief  Enables the specified LTDC interrupts.
  * @param  __HANDLE__  LTDC handle
  * @param __INTERRUPT__ Specify the LTDC interrupt sources to be enabled.
  *          This parameter can be any combination of the following values:
  *            @arg LTDC_IT_LI: Line Interrupt flag
  *            @arg LTDC_IT_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_IT_TE: Transfer Error interrupt flag
  *            @arg LTDC_IT_RR: Register Reload Interrupt Flag
  *            @arg LTDC_IT_CRC: LTDC CRC error Interrupt
  *            @arg LTDC_IT_FUW: FIFO Warning Underrun Interrupt
  * @retval None
  */
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define __HAL_LTDC_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER2 |= (__INTERRUPT__))
#else
#define __HAL_LTDC_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER |= (__INTERRUPT__))
#endif /* __ARM_FEATURE_CMSE & __ARM_FEATURE_CMSE == 3U */

/**
  * @brief  Disables the specified LTDC interrupts.
  * @param  __HANDLE__  LTDC handle
  * @param __INTERRUPT__ Specify the LTDC interrupt sources to be disabled.
  *          This parameter can be any combination of the following values:
  *            @arg LTDC_IT_LI: Line Interrupt flag
  *            @arg LTDC_IT_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_IT_TE: Transfer Error interrupt flag
  *            @arg LTDC_IT_RR: Register Reload Interrupt Flag
  *            @arg LTDC_IT_CRC: LTDC CRC error Interrupt
  *            @arg LTDC_IT_FUW: FIFO Underrun Warning Interrupt
  * @retval None
  */
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define __HAL_LTDC_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER2 &= ~(__INTERRUPT__))
#else
#define __HAL_LTDC_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER &= ~(__INTERRUPT__))
#endif /* __ARM_FEATURE_CMSE & __ARM_FEATURE_CMSE == 3U */

/**
  * @brief  Check whether the specified LTDC interrupt has occurred or not.
  * @param  __HANDLE__  LTDC handle
  * @param __INTERRUPT__ Specify the LTDC interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg LTDC_IT_LI: Line Interrupt flag
  *            @arg LTDC_IT_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_IT_TE: Transfer Error interrupt flag
  *            @arg LTDC_IT_RR: Register Reload Interrupt Flag
  *            @arg LTDC_IT_CRC: LTDC CRC error Interrupt
  *            @arg LTDC_IT_FUW: FIFO Underrun Warning Interrupt
  * @retval The state of INTERRUPT (SET or RESET).
  */
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define __HAL_LTDC_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER2 & (__INTERRUPT__))
#else
#define __HAL_LTDC_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER & (__INTERRUPT__))
#endif /* __ARM_FEATURE_CMSE & __ARM_FEATURE_CMSE == 3U */
/**
  * @}
  */

/* Include LTDC HAL Extension module */
#include "stm32n6xx_hal_ltdc_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup LTDC_Exported_Functions
  * @{
  */
/** @addtogroup LTDC_Exported_Functions_Group1
  * @{
  */
/* Initialization and de-initialization functions *****************************/
HAL_StatusTypeDef HAL_LTDC_Init(LTDC_HandleTypeDef *hltdc);
HAL_StatusTypeDef HAL_LTDC_DeInit(LTDC_HandleTypeDef *hltdc);
void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc);
void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef *hltdc);
void HAL_LTDC_ErrorCallback(LTDC_HandleTypeDef *hltdc);
void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *hltdc);
void HAL_LTDC_ReloadEventCallback(LTDC_HandleTypeDef *hltdc);
void HAL_LTDC_WarningEventCallback(LTDC_HandleTypeDef *hltdc);

/* Burst length function *****************************/
HAL_StatusTypeDef HAL_LTDC_ConfigBurstLength(LTDC_HandleTypeDef *hltdc, uint32_t BurstLength, uint32_t LayerIdx);

/* Underrun threshold function *****************************/
HAL_StatusTypeDef HAL_LTDC_ConfigUnderrunThreshold(LTDC_HandleTypeDef *hltdc, uint16_t Threshold);

/* Mirroring functions *****************************/
HAL_StatusTypeDef HAL_LTDC_ConfigMirror(LTDC_HandleTypeDef *hltdc, uint32_t Mirror, uint32_t LayerIdx);

/* Callbacks Register/UnRegister functions  ***********************************/
#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_LTDC_RegisterCallback(LTDC_HandleTypeDef *hltdc, HAL_LTDC_CallbackIDTypeDef CallbackID,
                                            pLTDC_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_LTDC_UnRegisterCallback(LTDC_HandleTypeDef *hltdc, HAL_LTDC_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @addtogroup LTDC_Exported_Functions_Group2
  * @{
  */
/* IO operation functions *****************************************************/
void  HAL_LTDC_IRQHandler(LTDC_HandleTypeDef *hltdc);
/**
  * @}
  */

/** @addtogroup LTDC_Exported_Functions_Group3
  * @{
  */
/* Peripheral Control functions ***********************************************/
HAL_StatusTypeDef HAL_LTDC_ConfigLayer(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_ConfigLayerFlexARGB(LTDC_HandleTypeDef *hltdc,
                                               const LTDC_LayerFlexARGBTypeDef *pLayerFlexARGB,
                                               uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_ConfigLayerFlexYUVSemiPlanar(LTDC_HandleTypeDef *hltdc,
                                                        LTDC_LayerFlexYUVSemiPlanarTypeDef *pLayerFlexYUVSemiPlanar,
                                                        uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_ConfigLayerFlexYUVFullPlanar(LTDC_HandleTypeDef *hltdc,
                                                        LTDC_LayerFlexYUVFullPlanarTypeDef *pLayerFlexYUVFullPlanar,
                                                        uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_ConfigLayerFlexYUVCoPlanar(LTDC_HandleTypeDef *hltdc,
                                                      const LTDC_LayerFlexYUVCoPlanarTypeDef *pLayerFlexYUVCoPlanar,
                                                      uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetWindowSize(LTDC_HandleTypeDef *hltdc, uint32_t XSize, uint32_t YSize, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetWindowPosition(LTDC_HandleTypeDef *hltdc, uint32_t X0, uint32_t Y0, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetPixelFormat(LTDC_HandleTypeDef *hltdc, uint32_t Pixelformat, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetAlpha(LTDC_HandleTypeDef *hltdc, uint32_t Alpha, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetAddress(LTDC_HandleTypeDef *hltdc, uint32_t Address, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetSemiPlanarAddress(LTDC_HandleTypeDef *hltdc,
                                                LTDC_LayerFlexYUVSemiPlanarTypeDef *pYUVSemiPlanarAddress,
                                                uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetFullPlanarAddress(LTDC_HandleTypeDef *hltdc,
                                                LTDC_LayerFlexYUVFullPlanarTypeDef *pYUVFullPlanarAddress,
                                                uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetSemiPlanarAddress_NoReload(LTDC_HandleTypeDef *hltdc,
                                                         LTDC_LayerFlexYUVSemiPlanarTypeDef *pYUVSemiPlanarAddress,
                                                         uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetFullPlanarAddress_NoReload(LTDC_HandleTypeDef *hltdc,
                                                         LTDC_LayerFlexYUVFullPlanarTypeDef *pYUVFullPlanarAddress,
                                                         uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetPitch(LTDC_HandleTypeDef *hltdc, uint32_t LinePitchInPixels, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_ConfigColorKeying(LTDC_HandleTypeDef *hltdc, uint32_t RGBValue, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_ConfigCLUT(LTDC_HandleTypeDef *hltdc, const uint32_t *pCLUT, uint32_t CLUTSize,
                                      uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_EnableColorKeying(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_DisableColorKeying(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_ConfigBlendingOrder(LTDC_HandleTypeDef *hltdc, uint32_t Order, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_EnableDefaultColor(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_DisableDefaultColor(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_EnableGammaCorrection(LTDC_HandleTypeDef *hltdc);
HAL_StatusTypeDef HAL_LTDC_DisableGammaCorrection(LTDC_HandleTypeDef *hltdc);
HAL_StatusTypeDef HAL_LTDC_ConfigGammaCorrection(LTDC_HandleTypeDef *hltdc, uint32_t GammaOnes,
                                                 uint32_t GammaTenths, uint32_t RGBComponent);
HAL_StatusTypeDef HAL_LTDC_EnableCLUT(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_DisableCLUT(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_ProgramLineEvent(LTDC_HandleTypeDef *hltdc, uint32_t Line);
HAL_StatusTypeDef HAL_LTDC_EnableDither(LTDC_HandleTypeDef *hltdc);
HAL_StatusTypeDef HAL_LTDC_DisableDither(LTDC_HandleTypeDef *hltdc);
HAL_StatusTypeDef HAL_LTDC_Reload(LTDC_HandleTypeDef *hltdc, uint32_t ReloadType);
HAL_StatusTypeDef HAL_LTDC_ReloadLayer(LTDC_HandleTypeDef *hltdc, uint32_t ReloadType, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetOutputDisplay(LTDC_HandleTypeDef *hltdc, uint32_t Display);
HAL_StatusTypeDef HAL_LTDC_SetExpectedCRC(LTDC_HandleTypeDef *hltdc, uint16_t ExpectedCRC);
HAL_StatusTypeDef HAL_LTDC_EnableCRC(LTDC_HandleTypeDef *hltdc);
HAL_StatusTypeDef HAL_LTDC_DisableCRC(LTDC_HandleTypeDef *hltdc);
HAL_StatusTypeDef HAL_LTDC_GetComputedCRC(LTDC_HandleTypeDef *hltdc, uint16_t *pComputedCRC);
HAL_StatusTypeDef HAL_LTDC_ConfigLayer_NoReload(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg,
                                                uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetWindowSize_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t XSize, uint32_t YSize,
                                                  uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetWindowPosition_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t X0, uint32_t Y0,
                                                      uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetPixelFormat_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t Pixelformat, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetAlpha_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t Alpha, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetAddress_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t Address, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_SetPitch_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t LinePitchInPixels, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_EnableColorKeying_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_DisableColorKeying_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_EnableCLUT_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);
HAL_StatusTypeDef HAL_LTDC_DisableCLUT_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx);

/**
  * @}
  */

/** @addtogroup LTDC_Exported_Functions_Group4
  * @{
  */
/* Peripheral State functions *************************************************/
HAL_LTDC_StateTypeDef HAL_LTDC_GetState(const LTDC_HandleTypeDef *hltdc);
uint32_t              HAL_LTDC_GetError(const LTDC_HandleTypeDef *hltdc);
/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @defgroup LTDC_Private_Macros LTDC Private Macros
  * @{
  */
#define LTDC_LAYER(__HANDLE__, __LAYER__)              ((LTDC_Layer_TypeDef *)((uint32_t)(\
                                                                               ((uint32_t)((__HANDLE__)->Instance))\
                                                                               + 0x100U + (0x100U*(__LAYER__)))))

#define IS_LTDC_LAYER(__LAYER__)                       ((__LAYER__) < MAX_LAYER)
#define IS_LTDC_PLANAR_LAYER(__LAYER__)                ((__LAYER__) == LTDC_LAYER_1)
#define IS_LTDC_HSPOL(__HSPOL__)                       (((__HSPOL__) == LTDC_HSPOLARITY_AL)\
                                                        || ((__HSPOL__) == LTDC_HSPOLARITY_AH))
#define IS_LTDC_VSPOL(__VSPOL__)                       (((__VSPOL__) == LTDC_VSPOLARITY_AL)\
                                                        || ((__VSPOL__) == LTDC_VSPOLARITY_AH))
#define IS_LTDC_DEPOL(__DEPOL__)                       (((__DEPOL__) == LTDC_DEPOLARITY_AL)\
                                                        || ((__DEPOL__) ==  LTDC_DEPOLARITY_AH))
#define IS_LTDC_PCPOL(__PCPOL__)                       (((__PCPOL__) == LTDC_PCPOLARITY_IPC)\
                                                        || ((__PCPOL__) ==  LTDC_PCPOLARITY_IIPC))
#define IS_LTDC_HSYNC(__HSYNC__)                       ((__HSYNC__)  <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_VSYNC(__VSYNC__)                       ((__VSYNC__)  <= LTDC_VERTICALSYNC)
#define IS_LTDC_AHBP(__AHBP__)                         ((__AHBP__)   <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_AVBP(__AVBP__)                         ((__AVBP__)   <= LTDC_VERTICALSYNC)
#define IS_LTDC_AAW(__AAW__)                           ((__AAW__)    <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_AAH(__AAH__)                           ((__AAH__)    <= LTDC_VERTICALSYNC)
#define IS_LTDC_TOTALW(__TOTALW__)                     ((__TOTALW__) <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_TOTALH(__TOTALH__)                     ((__TOTALH__) <= LTDC_VERTICALSYNC)
#define IS_LTDC_BLUEVALUE(__BBLUE__)                   ((__BBLUE__)  <= LTDC_COLOR)
#define IS_LTDC_GREENVALUE(__BGREEN__)                 ((__BGREEN__) <= LTDC_COLOR)
#define IS_LTDC_REDVALUE(__BRED__)                     ((__BRED__)   <= LTDC_COLOR)
#define IS_LTDC_BLENDING_FACTOR1(__BLENDING_FACTOR1__) (((__BLENDING_FACTOR1__) == LTDC_BLENDING_FACTOR1_CA) || \
                                                        ((__BLENDING_FACTOR1__) == LTDC_BLENDING_FACTOR1_PAxCA))
#define IS_LTDC_BLENDING_FACTOR2(__BLENDING_FACTOR1__) (((__BLENDING_FACTOR1__) == LTDC_BLENDING_FACTOR2_CA) || \
                                                        ((__BLENDING_FACTOR1__) == LTDC_BLENDING_FACTOR2_PAxCA))
#define IS_LTDC_PIXEL_FORMAT(__PIXEL_FORMAT__) \
  (((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_ARGB8888)  || \
   ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_ABGR8888)  || \
   ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_RGBA8888)  || \
   ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_BGRA8888)  || \
   ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_RGB565)    || \
   ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_BGR565)    || \
   ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_RGB888)    || \
   ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_ARGB1555)  || \
   ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_ARGB4444)  || \
   ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_L8)        || \
   ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_AL44)      || \
   ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_AL88))

#define IS_LTDC_ALPHA(__ALPHA__)                       ((__ALPHA__) <= LTDC_ALPHA)
#define IS_LTDC_HCONFIGST(__HCONFIGST__)               ((__HCONFIGST__) <= LTDC_STARTPOSITION)
#define IS_LTDC_HCONFIGSP(__HCONFIGSP__)               ((__HCONFIGSP__) <= LTDC_STOPPOSITION)
#define IS_LTDC_VCONFIGST(__VCONFIGST__)               ((__VCONFIGST__) <= LTDC_STARTPOSITION)
#define IS_LTDC_VCONFIGSP(__VCONFIGSP__)               ((__VCONFIGSP__) <= LTDC_STOPPOSITION)
#define IS_LTDC_CFBP(__CFBP__)                         ((__CFBP__) <= LTDC_COLOR_FRAME_BUFFER)
#define IS_LTDC_CFBLL(__CFBLL__)                       ((__CFBLL__) <= LTDC_COLOR_FRAME_BUFFER)
#define IS_LTDC_CFBLNBR(__CFBLNBR__)                   ((__CFBLNBR__) <= LTDC_LINE_NUMBER)
#define IS_LTDC_LIPOS(__LIPOS__)                       ((__LIPOS__) <= 0x7FFU)
#define IS_LTDC_RELOAD(__RELOADTYPE__)                 (((__RELOADTYPE__) == LTDC_RELOAD_IMMEDIATE) || \
                                                        ((__RELOADTYPE__) == LTDC_RELOAD_VERTICAL_BLANKING))

#define IS_LTDC_MIRROR(__MIRROR__)         (((__MIRROR__) == LTDC_MIRROR_HORIZONTAL)           ||\
                                            ((__MIRROR__) == LTDC_MIRROR_VERTICAL)             ||\
                                            ((__MIRROR__) == LTDC_MIRROR_HORIZONTAL_VERTICAL) ||\
                                            ((__MIRROR__) == LTDC_MIRROR_NONE))

#define IS_LTDC_ARGB_SIZE(__ARGB_SIZE__)               (((__ARGB_SIZE__) == LTDC_ARGB_PIXEL_SIZE_1_BYTE)         ||\
                                                        ((__ARGB_SIZE__) == LTDC_ARGB_PIXEL_SIZE_2_BYTES)        ||\
                                                        ((__ARGB_SIZE__) == LTDC_ARGB_PIXEL_SIZE_3_BYTES)        ||\
                                                        ((__ARGB_SIZE__) == LTDC_ARGB_PIXEL_SIZE_4_BYTES))

#define IS_LTDC_ARGB_COMPONENT_POSITION(__ARGB_POS__)         ((__ARGB_POS__) <= 0x1FU)
#define IS_LTDC_ARGB_COMPONENT_WIDTH(__ARGB_WIDTH__)         ((__ARGB_WIDTH__) <= 0xFU)
#define IS_LTDC_YUV_ORDER(__YUV_ORDER__)               (((__YUV_ORDER__) == LTDC_YUV_ORDER_LUMINANCE_FIRST)      ||\
                                                        ((__YUV_ORDER__) == LTDC_YUV_ORDER_CHROMINANCE_FIRST))

#define IS_LTDC_Y_ORDER(__Y_ORDER__)                 (((__Y_ORDER__) == LTDC_YUV_LUMINANCE_ORDER_ODD_FIRST)         ||\
                                                      ((__Y_ORDER__) == LTDC_YUV_LUMINANCE_ORDER_EVEN_FIRST))

#define IS_LTDC_UV_ORDER(__UV_ORDER__)                 (((__UV_ORDER__) == LTDC_YUV_CHROMIANCE_ORDER_U_FIRST)   ||\
                                                        ((__UV_ORDER__) == LTDC_YUV_CHROMIANCE_ORDER_V_FIRST))

#define IS_LTDC_Y_RANHGE(__Y_RANGE__)                 (((__Y_RANGE__) == LTDC_YUV_LUMINANCE_RESCALE_ENABLE)   ||\
                                                       ((__Y_RANGE__) == LTDC_YUV_LUMINANCE_RESCALE_DISABLE))

#define IS_LTDC_BLEND_ORDER(__BLEND_ORDER__)          (((__BLEND_ORDER__) == LTDC_BLENDING_ORDER_FOREGROUND)   ||\
                                                       ((__BLEND_ORDER__) == LTDC_BLENDING_ORDER_BACKGROUND))

#define IS_LTDC_RGB_COMPONENT(__RGB_COMPONENT__)      (((__RGB_COMPONENT__) == LTDC_RGB_COMPONENT_RED)          ||\
                                                       ((__RGB_COMPONENT__) == LTDC_RGB_COMPONENT_GREEN)        ||\
                                                       ((__RGB_COMPONENT__) == LTDC_RGB_COMPONENT_BLUE )        ||\
                                                       ((__RGB_COMPONENT__) == LTDC_RGB_COMPONENT_ALL))

#define IS_LTDC_DISPLAY(__LTDC_DISPLAY__)             (((__LTDC_DISPLAY__) == LTDC_OUT_RGB)        ||\
                                                       ((__LTDC_DISPLAY__) == LTDC_OUT_YUV_HDTV)   ||\
                                                       ((__LTDC_DISPLAY__) == LTDC_OUT_YUV_SDTV)   ||\
                                                       ((__LTDC_DISPLAY__) == LTDC_OUT_YVU_HDTV)   ||\
                                                       ((__LTDC_DISPLAY__) == LTDC_OUT_YVU_SDTV))

#define IS_LTDC_BURST_LENGTH(__BURST_LENGTH__)                       (((__BURST_LENGTH__) <= 0x10U) &&\
                                                                      ((__BURST_LENGTH__) >= 0x1U))

#define IS_LTDC_GAMMA_VALUE(__ONES__, __TENTHS__)      ((((__TENTHS__) + ((__ONES__) * 10U)) >= 4U)   &&\
                                                        (((__TENTHS__) + ((__ONES__) * 10U)) <= 29U))

#define IS_LTDC_YUV2RGBCONVERTOR(__CONVERTOR__)  (((__CONVERTOR__) == LTDC_YUV2RGBCONVERTOR_BT709_FULL_RANGE)      ||\
                                                  ((__CONVERTOR__) == LTDC_YUV2RGBCONVERTOR_BT709_REDUCED_RANGE)   ||\
                                                  ((__CONVERTOR__) == LTDC_YUV2RGBCONVERTOR_BT601_FULL_RANGE)      ||\
                                                  ((__CONVERTOR__) == LTDC_YUV2RGBCONVERTOR_BT601_REDUCED_RANGE))

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup LTDC_Private_Functions LTDC Private Functions
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

#endif /* LTDC */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_LTDC_H */

