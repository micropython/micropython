/**
  ******************************************************************************
  * @file    stm32n6xx_hal_dcmipp.h
  * @author  MCD Application Team
  * @brief   Header file of DCMIPP HAL module.
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
#ifndef __STM32N6xx_HAL_DCMIPP_H
#define __STM32N6xx_HAL_DCMIPP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

#if defined (DCMIPP)

/** @addtogroup DCMIPP DCMIPP
  * @brief DCMIPP HAL module driver
  * @{
  */

/* Exported types ----------------------------------------------------------------------------------------------------*/
/** @defgroup DCMIPP_Exported_Types DCMIPP Exported Types
  * @brief    DCMIPP Exported Types
  * @{
  */
#define MAX_DATATYPE_NB  7U  /*!< DCMIPP CSI maximum number of data types  */
/**
  * @brief   DCMIPP Embedded Synchronisation Unmask codes structure definition
  */
typedef struct
{
  uint8_t FrameStartUnmask; /*!< Specifies the frame start delimiter unmask. */
  uint8_t LineStartUnmask;  /*!< Specifies the line start delimiter unmask.  */
  uint8_t LineEndUnmask;    /*!< Specifies the line end delimiter unmask.    */
  uint8_t FrameEndUnmask;   /*!< Specifies the frame end delimiter unmask.   */
} DCMIPP_EmbeddedSyncUnmaskTypeDef;

/**
  * @brief   DCMIPP Embedded Synchronisation codes structure definition (CCIR656)
  */
typedef struct
{
  uint8_t FrameStartCode;  /*!< Specifies the code of the frame start delimiter. */
  uint8_t LineStartCode;   /*!< Specifies the code of the line start delimiter.  */
  uint8_t LineEndCode;     /*!< Specifies the code of the line end delimiter.    */
  uint8_t FrameEndCode;    /*!< Specifies the code of the frame end delimiter.   */
} DCMIPP_EmbeddedSyncCodesTypeDef;

/**
  * @brief  HAL DCMIPP CSI PIPE configuration structure definition
  */
typedef struct
{
  uint32_t DataTypeMode ; /*!< Configures the Data Type Mode
                               This parameter can be a value from @ref DCMIPP_DataTypeMode    */
  uint32_t DataTypeIDA ;  /*!< Configures the Data type selection ID-A
                               This parameter can be a value from @ref DCMIPP_DataType        */
  uint32_t DataTypeIDB ;  /*!< Configures the Data type selection ID-B
                               This parameter can be a value from @ref DCMIPP_DataType        */
} DCMIPP_CSI_PIPE_ConfTypeDef;
/**
  * @brief  HAL DCMIPP CSI configuration structure definition
  */
typedef struct
{
  uint32_t NumberOfLanes;     /*!< Configures the Number of Lanes
                                   This parameter can be a value from @ref DCMIPP_CSI_Number_Of_Lanes */
  uint32_t DataLaneMapping;   /*!< Configures the Data Lane Mapping
                                   This parameter can be a value from @ref DCMIPP_CSI_DataLaneMapping */
  uint32_t PHYBitrate;        /*!< Configures the Phy Bitrate
                                   This parameter can be a value from @ref DCMIPP_CSI_PHYBitRate      */
} DCMIPP_CSI_ConfTypeDef;
/**
  * @brief  HAL DCMIPP CSI Virtual Channel Filtering configuration structure definition
  */
typedef struct
{
  uint32_t DataTypeNB;                        /*!< Configures the Data Type number
                                                   This parameter can be a value from @ref DCMIPP_CSI_DataTypeID     */
  uint32_t DataTypeClass[MAX_DATATYPE_NB];    /*!< Configures the Data Type Class
                                                   This parameter can be a value between 0 and 63                    */
  uint32_t DataTypeFormat[MAX_DATATYPE_NB];   /*!< Configures the Data Type Class
                                                   This parameter can be a value from @ref DCMIPP_CSI_DataTypeFormat */
} DCMIPP_CSI_VCFilteringConfTypeDef;

/**
  * @brief  HAL DCMIPP CSI Timer configuration structure definition
  */
typedef struct
{
  uint32_t VirtualChannel;       /*!< The Virtual Channel ID
                                      This parameter can be a value from @ref DCMIPP_Virtual_Channel      */
  uint32_t StartPoint;           /*!< The Start Point of the counter
                                      This parameter can be a value from @ref DCMIPP_CSI_Timer_StartPoint */
  uint32_t Count;                /*!< Number of clock cycle to count from the start point
                                      This parameter can be a value between 0 and 0x1FFFFFF               */
} DCMIPP_CSI_TimerConfTypeDef;
/**
  * @brief  HAL DCMIPP CSI LineByte Counter configuration structure definition
  */
typedef struct
{
  uint32_t VirtualChannel;   /*!< Configures the Virtual Channel ID
                                  This parameter can be a value from @ref DCMIPP_Virtual_Channel */
  uint32_t LineCounter;      /*!< Configures the Line Number
                                  This parameter can be a value between 0 and 65535              */
  uint32_t ByteCounter;      /*!< Configures the Byte Number
                                  This parameter can be a value between 0 and 65535              */
} DCMIPP_CSI_LineByteCounterConfTypeDef;

/**
  * @brief  HAL DCMIPP Parallel configuration structure definition
  */
typedef struct
{
  uint32_t  Format;                             /*!< Configures the DCMIPP Format
                                                  This parameter can be one value of @ref DCMIPP_Format              */
  uint32_t  VSPolarity;                         /*!< Specifies the Vertical synchronization polarity: High or Low.
                                                  This parameter can be a value of @ref DCMIPP_VSYNC_Polarity        */
  uint32_t  HSPolarity;                         /*!< Specifies the Horizontal synchronization polarity: High or Low.
                                                  This parameter can be a value of @ref DCMIPP_HSYNC_Polarity        */
  uint32_t  PCKPolarity;                        /*!< Specifies the Pixel clock polarity: Falling or Rising.
                                                  This parameter can be a value of @ref DCMIPP_PIXCK_Polarity        */
  uint32_t  ExtendedDataMode ;                  /*!< Specifies the data width: 8-bit, 10-bit, 12-bit 14-bit or 16-bits.
                                                  This parameter can be a value of @ref DCMIPP_Extended_Data_Mode    */
  uint32_t  SynchroMode;                        /*!< Specifies the Synchronization Mode: Hardware or Embedded.
                                                  This parameter can be a value of @ref DCMIPP_Synchronization_Mode  */
  DCMIPP_EmbeddedSyncCodesTypeDef SynchroCodes; /*!< Specifies the code of the line/frame start delimiter and the
                                                  line/frame end delimiter                                           */
  uint32_t  SwapBits;                           /*!< Enable or Disable the Swap Bits.
                                                  This parameter can be a value of @ref DCMIPP_SWAP_BITS             */
  uint32_t  SwapCycles;                         /*!<  Enable or Disable the Swap Cycles.
                                                  This parameter can be a value of @ref DCMIPP_SWAP_CYCLES           */
} DCMIPP_ParallelConfTypeDef;

/**
  * @brief  HAL DCMIPP Pipe configuration structure definition
  */
typedef struct
{
  uint32_t FrameRate;         /*!< Configures the DCMIPP Pipe Frame Rate
                                   This parameter can be one value of @ref DCMIPP_Frame_Rates           */
  uint32_t PixelPipePitch;    /*!< Configures the DCMIPP Pixel Pipe Pitch
                                   This parameter can be one value from                                 */
  uint32_t PixelPackerFormat; /*!< Configures the DCMIPP Pixel Pipe Format
                                   This parameter can be one value from @ref DCMIPP_Pixel_Packer_Format */
} DCMIPP_PipeConfTypeDef;

/**
  * @brief  HAL DCMIPP IPPLUG configuration structure definition
  */
typedef struct
{
  uint32_t Client;                      /*!< Configures the DCMIPP IPPLUG Client
                                             This parameter can be a value from @ref DCMIPP_IPPLUG_Client      */
  uint32_t MemoryPageSize;              /*!< Configures the DCMIPP IPPLUG Memory page size
                                             This parameter can be a value from @ref DCMIPP_Memory_Page_Size   */
  uint32_t Traffic;                     /*!< Configures the DCMIPP IPPLUG Traffic
                                             This parameter can be a value from @ref DCMIPP_Traffic_Burst_Size */
  uint32_t MaxOutstandingTransactions ; /*!< Configures the DCMIPP IPPLUG Maximum outstanding transactions
                                             This parameter can be a value from
                                             DCMIPP_Maximum_Outstanding_Transactions                           */
  uint32_t DPREGStart;                  /*!< Configures the End word of the FIFO of Clientx
                                             This parameter can be a value between 0 and  0x3FF                */
  uint32_t DPREGEnd;                    /*!< Configures the Start word of the FIFO of Clientx
                                             This parameter can be a value between 0 and  0x3FF                */
  uint32_t WLRURatio;                   /*!< Configures the DCMIPP Ratio for WLRU arbitration
                                             This parameter can be a value between 0 and 15                    */
} DCMIPP_IPPlugConfTypeDef;

/**
  * @brief  HAL DCMIPP Crop configuration structure definition
  */
typedef struct
{
  uint32_t VStart;   /*!< Configures the DCMIPP Crop Vertical Start
                          This parameter can be one value between 0 and 4095          */
  uint32_t HStart;   /*!< Configures the DCMIPP Crop Horizontal Start
                          This parameter can be one value between 0 and 4095          */
  uint32_t VSize;    /*!< Configures the DCMIPP Crop Vertical Size
                          This parameter can be one value between 0 and 4095          */
  uint32_t HSize;    /*!< Configures the DCMIPP Crop Horizontal Size
                          This parameter can be one value between 1 and 4095          */
  uint32_t PipeArea; /*!< Configures the DCMIPP Crop Area for the pipe0
                          This parameter can be one value of @ref DCMIPP_Crop_Area    */
} DCMIPP_CropConfTypeDef;

typedef struct
{
  uint32_t VStart;    /*!< Configures the DCMIPP Statistic Extraction Vertical Start
                           This parameter can be one value between 0 and 4095                     */
  uint32_t HStart;    /*!< Configures the DCMIPP Statistic Extraction Horizontal Start
                           This parameter can be one value between 0 and 4095                     */
  uint32_t VSize;     /*!< Configures the DCMIPP Statistic Extraction Vertical Size
                           This parameter can be one value between 0 and 4095                     */
  uint32_t HSize;     /*!< Configures the DCMIPP Statistic Extraction Horizontal Size
                           This parameter can be one value between 0 and 4095                     */
} DCMIPP_StatisticExtractionAreaConfTypeDef;

typedef struct
{
  uint32_t Mode;      /*!< Configures the DCMIPP Statistic Extraction Mode
                           This parameter can be a value from @ref DCMIPP_Statistics_Extraction_Mode   */
  uint32_t Source;    /*!< Configures the DCMIPP Statistic Extraction Source
                           This parameter can be a value from @ref DCMIPP_Statistics_Extraction_Source */
  uint32_t Bins;      /*!< Configures the DCMIPP Statistic Extraction Bins
                           This parameter can be a value from @ref DCMIPP_Statistics_Extraction_Bins   */
} DCMIPP_StatisticExtractionConfTypeDef;


typedef struct
{
  uint8_t ShiftRed;        /*!< Configures the DCMIPP Exposure Shift Red
                                This parameter can be one value between 0 and 7    */
  uint8_t MultiplierRed;   /*!< Configures the DCMIPP Exposure Multiplier Red
                                This parameter can be one value between 0 and 127 */
  uint8_t ShiftGreen;      /*!< Configures the DCMIPP Exposure Shift Green
                                This parameter can be one value between 0 and 7    */
  uint8_t MultiplierGreen; /*!< Configures the DCMIPP Exposure Multiplier Green
                                This parameter can be one value between 0 and 127 */
  uint8_t ShiftBlue;       /*!< Configures the DCMIPP Exposure Shift Blue
                                This parameter can be one value between 0 and 7    */
  uint8_t MultiplierBlue;  /*!< Configures the DCMIPP Exposure Multiplier Blue
                                This parameter can be one value between 0 and 127 */
} DCMIPP_ExposureConfTypeDef;

typedef struct
{
  uint8_t LUM_0;    /*!< Luminance increase for input luminance of 0
                         This parameter can be one value between 0 and 31 */
  uint8_t LUM_32;   /*!< Luminance increase for input luminance of 32
                         This parameter can be one value between 0 and 31 */
  uint8_t LUM_64;   /*!< Luminance increase for input luminance of 64
                         This parameter can be one value between 0 and 31 */
  uint8_t LUM_96;   /*!< Luminance increase for input luminance of 96
                         This parameter can be one value between 0 and 31 */
  uint8_t LUM_128;  /*!< Luminance increase for input luminance of 128
                         This parameter can be one value between 0 and 31 */
  uint8_t LUM_160;  /*!< Luminance increase for input luminance of 160
                         This parameter can be one value between 0 and 31 */
  uint8_t LUM_192;  /*!< Luminance increase for input luminance of 192
                         This parameter can be one value between 0 and 31 */
  uint8_t LUM_224;  /*!< Luminance increase for input luminance of 224
                         This parameter can be one value between 0 and 31 */
  uint8_t LUM_256;  /*!< Luminance increase for input luminance of 256
                         This parameter can be one value between 0 and 31 */
} DCMIPP_ContrastConfTypeDef;

typedef struct
{
  uint32_t VLineStrength;  /*!< Strength of the vertical line detection
                                This parameter can be a value from @ref DCMIPP_RAWBayer2RGB_Strength */
  uint32_t HLineStrength;  /*!< Strength of the horizontal line detection
                                This parameter can be a value from @ref DCMIPP_RAWBayer2RGB_Strength */
  uint32_t RawBayerType;   /*!< Raw Bayer type
                                This parameter can be a value from @ref DCMIPP_RAWBayer2RGB_Type     */
  uint32_t PeakStrength;   /*!< Strength of the peak detection
                                This parameter can be a value from @ref DCMIPP_RAWBayer2RGB_Strength */
  uint32_t EdgeStrength;   /*!< Strength of the edge detection
                                This parameter can be a value from @ref DCMIPP_RAWBayer2RGB_Strength */
} DCMIPP_RawBayer2RGBConfTypeDef;

/**
   * @brief  Color Conversion parameters
   */
typedef struct
{
  FunctionalState ClampOutputSamples;  /*!< Clamp the output samples depending on type
                                            This parameter can be ENABLE or DISABLE */
  uint8_t         OutputSamplesType;   /*!< Output samples type used while clamp is activated
                                            This parameter can be a value from @ref DCMIPP_OutputSamplesType */
  int16_t RR;                          /*!< Coefficient Row 1 Column 1 of the matrix
                                            This parameter can be a value from -1023 to 1023 */
  int16_t RG;                          /*!< Coefficient Row 1 Column 2 of the matrix
                                            This parameter can be a value from -1023 to 1023 */
  int16_t RB;                          /*!< Coefficient Row 1 Column 3 of the matrix
                                            This parameter can be a value from -511 to 511   */
  int16_t RA;                          /*!< Coefficient Row 1 of the added Column
                                            This parameter can be a value from -511 to 511   */
  int16_t GR;                          /*!< Coefficient Row 2 Column 1 of the matrix
                                            This parameter can be a value from -1023 to 1023 */
  int16_t GG;                          /*!< Coefficient Row 2 Column 2 of the matrix
                                            This parameter can be a value from -1023 to 1023 */
  int16_t GB;                          /*!< Coefficient Row 2 Column 3 of the matrix
                                            This parameter can be a value from -511 to 511   */
  int16_t GA;                          /*!< Coefficient Row 2 of the added Column
                                            This parameter can be a value from -511 to 511   */
  int16_t BR;                          /*!< Coefficient Row 3 Column 1 of the matrix
                                            This parameter can be a value from -1023 to 1023 */
  int16_t BG;                          /*!< Coefficient Row 3 Column 2 of the matrix
                                             This parameter can be a value -1023 to 1023     */
  int16_t BB;                          /*!< Coefficient Row 3 Column 3 of the matrix
                                            This parameter can be a value from -511 to 511   */
  int16_t BA;                          /*!< Coefficient Row 3 of the added Column
                                            This parameter can be a value from -511 to 511   */
} DCMIPP_ColorConversionConfTypeDef;

/**
   * @brief  Region Of Interest Structure
   */
typedef struct
{
  uint32_t VStart;           /*!< Configures the DCMIPP Region Of Interest Vertical Start
                                  This parameter can be one value between 0 and 4095                      */
  uint32_t HStart;           /*!< Configures the DCMIPP Region Of Interest Horizontal Start
                                  This parameter can be one value between 0 and 4095                      */
  uint32_t VSize;            /*!< Configures the DCMIPP Region Of Interest Vertical Size
                                  This parameter can be one value between 0 and 4095                      */
  uint32_t HSize;            /*!< Configures the DCMIPP Region Of Interest Horizontal Size
                                  This parameter can be one value between 0 and 4095                      */
  uint32_t LineSizeWidth;    /*!< Configures the DCMIPP Region Of Interest Line Size Width
                                  This parameter can be a value from @ref DCMIPP_Region_Of_Interest_Line_Width */
  uint32_t RegionOfInterest; /*!< Configures the DCMIPP Region Of Interest
                                  This parameter can be a value from @ref DCMIPP_Region_Of_Interest       */
  uint8_t  ColorLineRed;     /*!< Configures the DCMIPP Region Of Interest Line Color : Red Component
                                  This parameter can be one value between 0 and 4095                      */
  uint8_t  ColorLineGreen;   /*!< Configures the DCMIPP Region Of Interest Line Color : Green Component
                                  This parameter can be one value between 0 and 4095                      */
  uint8_t  ColorLineBlue;    /*!< Configures the DCMIPP Region Of Interest Line Color : Blue Component
                                  This parameter can be one value between 0 and 4095                      */
} DCMIPP_RegionOfInterestConfTypeDef;

/**
   * @brief  Black Level parameters
   */
typedef struct
{
  uint8_t RedCompBlackLevel;    /*!< Black value register to red component
                                  This parameter can be one value between 0 and 255 */
  uint8_t GreenCompBlackLevel;  /*!< Black value register to green component
                                  This parameter can be one value between 0 and 255 */
  uint8_t BlueCompBlackLevel;   /*!< Black value register to blue component
                                  This parameter can be one value between 0 and 255 */
} DCMIPP_BlackLevelConfTypeDef;

/**
  * @brief  HAL DCMIPP
  */
typedef struct
{
  uint32_t VSize;       /*!< Configures the DCMIPP Downsize Vertical Size
                             This parameter can be one value between 0 and 4095     */
  uint32_t HSize;       /*!< Configures the DCMIPP Downsize Horizontal Size
                             This parameter can be one value between 0 and 4095     */
  uint32_t VRatio;      /*!< Configures the DCMIPP Downsize Vertical Ratio
                             This parameter can be one value between 0 and 4095     */
  uint32_t HRatio;      /*!< Configures the DCMIPP Downsize Horizontal Ratio
                             This parameter can be one value between 0 and 4095     */
  uint32_t VDivFactor;  /*!< Configures the DCMIPP Downsize Vertical Division Factor
                             This parameter can be one value between 0 and 4095     */
  uint32_t HDivFactor;  /*!< Configures the DCMIPP Downsize Horizontal Division Factor
                             This parameter can be one value between 0 and 4095     */
} DCMIPP_DownsizeTypeDef;

typedef struct
{
  uint32_t VRatio;  /*!< Configures the DCMIPP Downsize Vertical Size
                         This parameter can be  a value from @ref DCMIPP_PIPE_Vertical_Decimation_Ratio   */
  uint32_t HRatio;  /*!< Configures the DCMIPP Downsize Vertical Size
                         This parameter can be  a value from @ref DCMIPP_PIPE_Horizontal_Decimation_Ratio */
} DCMIPP_DecimationConfTypeDef;

typedef struct
{
  uint32_t YAddress;     /*!< Y Frame Buffer address  */
  uint32_t UVAddress;    /*!< UV Frame Buffer address */
} DCMIPP_SemiPlanarDstAddressTypeDef;

typedef struct
{
  uint32_t YAddress;    /*!< Y Frame Buffer address */
  uint32_t UAddress;    /*!< U Frame Buffer address */
  uint32_t VAddress;    /*!< V Frame Buffer address */
} DCMIPP_FullPlanarDstAddressTypeDef;

/**
  * @brief  HAL DCMIPP State enumeration definition
  */
typedef enum
{
  HAL_DCMIPP_STATE_RESET = 0x00U,  /*!< DCMIPP not yet initialized or disabled  */
  HAL_DCMIPP_STATE_INIT  = 0x01U,  /*!< DCMIPP initialized                      */
  HAL_DCMIPP_STATE_READY = 0x02U,  /*!< DCMIPP configured and ready for use     */
  HAL_DCMIPP_STATE_BUSY  = 0x03U,  /*!< DCMIPP internal processing is ongoing   */
  HAL_DCMIPP_STATE_ERROR = 0x04U,  /*!< DCMIPP state error                      */
} HAL_DCMIPP_StateTypeDef;

/**
  * @brief  HAL DCMIPP Pipe State enumeration definition
  */

typedef enum
{
  HAL_DCMIPP_PIPE_STATE_RESET             = 0x00U,  /*!< DCMIPP Pipe not yet initialized or disabled */
  HAL_DCMIPP_PIPE_STATE_READY             = 0x01U,  /*!< DCMIPP Pipe initialized and ready for use   */
  HAL_DCMIPP_PIPE_STATE_BUSY              = 0x02U,  /*!< DCMIPP internal processing is ongoing       */
  HAL_DCMIPP_PIPE_STATE_SUSPEND           = 0x03U,  /*!< DCMIPP pipe process is suspended            */
  HAL_DCMIPP_PIPE_STATE_ERROR             = 0x04U,  /*!< DCMIPP pipe error state                     */
} HAL_DCMIPP_PipeStateTypeDef;

#if (USE_HAL_DCMIPP_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL DCMIPP common Callback ID enumeration definition
  */
typedef enum
{
  HAL_DCMIPP_MSPINIT_CB_ID                = 0x00U,    /*!< DCMIPP MspInit callback ID                 */
  HAL_DCMIPP_MSPDEINIT_CB_ID              = 0x01U,    /*!< DCMIPP MspDeInit callback ID               */
  HAL_DCMIPP_ERROR_CB_ID                  = 0x02U,    /*!< DCMIPP Error callback ID                   */
} HAL_DCMIPP_CallbackIDTypeDef;

/**
  * @brief  HAL DCMIPP pipe Callback ID enumeration definition
  */
typedef enum
{
  HAL_DCMIPP_PIPE_LIMIT_EVENT_CB_ID       = 0x01U,    /*!< DCMIPP Pipe Limit event callback ID        */
  HAL_DCMIPP_PIPE_LINE_EVENT_CB_ID        = 0x02U,    /*!< DCMIPP Pipe Line event callback ID         */
  HAL_DCMIPP_PIPE_FRAME_EVENT_CB_ID       = 0x03U,    /*!< DCMIPP Pipe Frame event callback ID        */
  HAL_DCMIPP_PIPE_VSYNC_EVENT_CB_ID       = 0x04U,    /*!< DCMIPP Pipe Vsync event callback ID        */
  HAL_DCMIPP_PIPE_ERROR_CB_ID             = 0x05U,    /*!< DCMIPP Pipe Error callback ID              */
} HAL_DCMIPP_PIPE_CallbackIDTypeDef;
#endif /* USE_HAL_DCMIPP_REGISTER_CALLBACKS */

/**
  * @brief  HAL DCMIPP handle structures definition
  */
#if (USE_HAL_DCMIPP_REGISTER_CALLBACKS == 1)
typedef struct __DCMIPP_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_DCMIPP_REGISTER_CALLBACKS */
{
  DCMIPP_TypeDef                    *Instance;                                             /*!< Register the DCMIPP base
                                                                                                address               */
  __IO HAL_DCMIPP_StateTypeDef      State;                                                 /*!< DCMIPP state          */
  __IO HAL_DCMIPP_PipeStateTypeDef  PipeState[DCMIPP_NUM_OF_PIPES];                        /*!< DCMIPP Pipes state    */
  __IO uint32_t                     ErrorCode;                                             /*!< DCMIPP Error code     */
#if (USE_HAL_DCMIPP_REGISTER_CALLBACKS == 1)
  void (* PIPE_FrameEventCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe); /*!< DCMIPP Pipe Frame Event
                                                                                                Callback              */
  void (* PIPE_VsyncEventCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe); /*!< DCMIPP Pipe Vsync Event
                                                                                                Callback              */
  void (* PIPE_LineEventCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);  /*!< DCMIPP Pipe Line Event
                                                                                                Callback              */
  void (* PIPE_LimitEventCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe); /*!< DCMIPP Pipe Limit Event
                                                                                                Callback              */
  void (* PIPE_ErrorCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);      /*!< DCMIPP Pipe Error
                                                                                                Callback              */
  void (* ErrorCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp);                         /*!< DCMIPP Error Callback  */
  void (* MspInitCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp);                       /*!< DCMIPP Msp Init
                                                                                                Callback              */
  void (* MspDeInitCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp);                     /*!< DCMIPP Msp DeInit
                                                                                                Callback              */
  void (* StartOfFrameEventCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp, uint32_t VirtualChannel); /*!< DCMIPP CSI
                                                                                        Start Of Frame Event Callback */
  void (* EndOfFrameEventCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp, uint32_t VirtualChannel); /*!< DCMIPP CSI
                                                                                          End Of Frame Event Callback */
  void (* TimerCounterEventCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp, uint32_t Timer); /*!< DCMIPP CSI
                                                                                         Timer Counter Event Callback */
  void (* LineByteEventCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp, uint32_t Counter); /*!< DCMIPP CSI Line/Byte
                                                                                                 Event Callback       */
  void (* LineErrorCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp, uint32_t DataLane); /*!< DCMIPP CSI Line Error
                                                                                              Event Callback          */
  void (* ClockChangerFifoFullEventCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp); /*!< DCMIPP CSI Clock changer
                                                                                           Fifo Event Callback        */
  void (* ShortPacketDetectionEventCallback)(struct __DCMIPP_HandleTypeDef *hdcmipp); /*!< DCMIPP CSI Short Packet
                                                                                           detection Event Callback   */
#endif  /* USE_HAL_DCMIPP_REGISTER_CALLBACKS */
} DCMIPP_HandleTypeDef;

#if (USE_HAL_DCMIPP_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL DCMIPP Callback pointer definition
  */
typedef void (*pDCMIPP_CallbackTypeDef)(DCMIPP_HandleTypeDef *hdcmipp); /*!< Pointer to a DCMIPP common callback
                                                                              function */
typedef void (*pDCMIPP_PIPE_CallbackTypeDef)(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe); /*!< Pointer to a DCMIPP
                                                                                               Pipe callback function */
#endif /* USE_HAL_DCMIPP_REGISTER_CALLBACKS */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup DCMIPP_Exported_Constants DCMIPP Exported Constants
  * @brief    DCMIPP Exported constants
  * @{
  */

/** @defgroup DCMIPP_Pipes DCMIPP Pipes
  * @{
  */
#define  DCMIPP_PIPE0      0U     /*!< DCMIPP Pipe0 (Dump pipe)       */
#define  DCMIPP_PIPE1      1U     /*!< DCMIPP Pipe1 (Main Pipe)       */
#define  DCMIPP_PIPE2      2U     /*!< DCMIPP Pipe2 (Ancillary pipe)  */
/**
  * @}
  */

/** @defgroup DCMIPP_Error_Codes DCMIPP Error Codes
  * @{
  */
#define HAL_DCMIPP_ERROR_NONE            (0x00000000U)             /*!< No error                  */
#define HAL_DCMIPP_ERROR_AXI_TRANSFER    (0x00000001U)             /*!< IPPLUG AXI Transfer error */
#define HAL_DCMIPP_ERROR_PARALLEL_SYNC   (0x00000002U)             /*!< Synchronization error     */
#define HAL_DCMIPP_ERROR_PIPE0_LIMIT     (0x00000004U)             /*!< Limit error on pipe0      */
#define HAL_DCMIPP_ERROR_PIPE0_OVR       (0x00000008U)             /*!< Overrun error on pipe0    */
#define HAL_DCMIPP_ERROR_PIPE1_OVR       (0x00000010U)             /*!< Overrun error on pipe1    */
#define HAL_DCMIPP_ERROR_PIPE2_OVR       (0x00000020U)             /*!< Overrun error on pipe2    */

#if (USE_HAL_DCMIPP_REGISTER_CALLBACKS == 1)
#define  HAL_DCMIPP_ERROR_INVALID_CALLBACK ((uint32_t)0x00000040U) /*!< Invalid Callback error  */
#endif /* USE_HAL_DCMIPP_REGISTER_CALLBACKS */

#define HAL_DCMIPP_CSI_ERROR_NONE            (0x00000080U)       /*!< No error                               */
#define HAL_DCMIPP_CSI_ERROR_SYNC            (0x00000100U)       /*!< Synchronization error                  */
#define HAL_DCMIPP_CSI_ERROR_WDG             (0x00000200U)       /*!< Watchdog Error                         */
#define HAL_DCMIPP_CSI_ERROR_SPKT            (0x00000400U)       /*!< Shorter Packet than expected Error     */
#define HAL_DCMIPP_CSI_ERROR_DATA_ID         (0x00000800U)       /*!< Data ID Information Error              */
#define HAL_DCMIPP_CSI_ERROR_CECC            (0x00001000U)       /*!< Corrected ECC Error                    */
#define HAL_DCMIPP_CSI_ERROR_ECC             (0x00002000U)       /*!< ECC Error                              */
#define HAL_DCMIPP_CSI_ERROR_CRC             (0x00004000U)       /*!< CRC Error                              */
#define HAL_DCMIPP_CSI_ERROR_DPHY_CTRL       (0x00008000U)       /*!< Error Control on data line (0 OR 1)    */
#define HAL_DCMIPP_CSI_ERROR_DPHY_LP_SYNC    (0x00010000U)       /*!< Low-Power data transmssion Synchronisation
                                                                      Error on data line (0 OR 1)   */
#define HAL_DCMIPP_CSI_ERROR_DPHY_ESCAPE     (0x00020000U)       /*!< Escape Entry Error on data line (0 OR 1) */
#define HAL_DCMIPP_CSI_ERROR_SOT_SYNC        (0x00040000U)       /*!< SOT Synchronisation Error on data line(0 OR 1) */
#define HAL_DCMIPP_CSI_ERROR_SOT             (0x00080000U)       /*!< SOT Error on data line (0 OR 1)        */
/**
  * @}
  */

/** @defgroup DCMIPP_Capture_Mode DCMIPP Capture Mode
  * @{
  */
#define  DCMIPP_MODE_CONTINUOUS   0U                      /*!< DCMIPP continuous mode (preview) */
#define  DCMIPP_MODE_SNAPSHOT     DCMIPP_P0FCTCR_CPTMODE  /*!< DCMIPP snapshot mode             */
/**
  * @}
  */

/**
  * @defgroup  DCMIPP_modes (DCMI or CSI): modes are exclusive
  * @{
  */
#define  DCMIPP_PARALLEL_MODE   (0x00U << DCMIPP_CMCR_INSEL_Pos)  /*!< DCMIPP Parallel (DCMI) mode */
#define  DCMIPP_SERIAL_MODE     (0x01U << DCMIPP_CMCR_INSEL_Pos)  /*!< DCMIPP CSI mode             */
/**
  * @}
  */

/** @defgroup DCMIPP_IPPLUG_Client DCMIPP IPPLUG Client
  * @{
  */
#define DCMIPP_CLIENT1  1U /*!< Client 1 identifier */
#define DCMIPP_CLIENT2  2U /*!< Client 2 identifier */
#define DCMIPP_CLIENT3  3U /*!< Client 3 identifier */
#define DCMIPP_CLIENT4  4U /*!< Client 4 identifier */
#define DCMIPP_CLIENT5  5U /*!< Client 5 identifier */
/**
  * @}
  */

/** @defgroup DCMIPP_Traffic_Burst_Size DCMIPP Traffic Burst Size
  * @{
  */
#define DCMIPP_TRAFFIC_BURST_SIZE_8BYTES   0U                                   /*!< Traffic Burst size 8 Bytes   */
#define DCMIPP_TRAFFIC_BURST_SIZE_16BYTES  (0x01U << DCMIPP_IPC1R1_TRAFFIC_Pos) /*!< Traffic Burst size 16 Bytes  */
#define DCMIPP_TRAFFIC_BURST_SIZE_32BYTES  (0x02U << DCMIPP_IPC1R1_TRAFFIC_Pos) /*!< Traffic Burst size 32 Bytes  */
#define DCMIPP_TRAFFIC_BURST_SIZE_64BYTES  (0x03U << DCMIPP_IPC1R1_TRAFFIC_Pos) /*!< Traffic Burst size 64 Bytes  */
#define DCMIPP_TRAFFIC_BURST_SIZE_128BYTES (0x04U << DCMIPP_IPC1R1_TRAFFIC_Pos) /*!< Traffic Burst size 128 Bytes */
/**
  * @}
  */

/** @defgroup DCMIPP_Memory_Page_Size DCMIPP Memory Page Size
  * @{
  */
#define DCMIPP_MEMORY_PAGE_SIZE_64BYTES  0U                                     /*!< Memory Page size 64 Bytes  */
#define DCMIPP_MEMORY_PAGE_SIZE_128BYTES (0x01U << DCMIPP_IPGR1_MEMORYPAGE_Pos) /*!< Memory Page size 128 Bytes */
#define DCMIPP_MEMORY_PAGE_SIZE_256BYTES (0x02U << DCMIPP_IPGR1_MEMORYPAGE_Pos) /*!< Memory Page size 256 Bytes */
#define DCMIPP_MEMORY_PAGE_SIZE_512BYTES (0x03U << DCMIPP_IPGR1_MEMORYPAGE_Pos) /*!< Memory Page size 512 Bytes */
#define DCMIPP_MEMORY_PAGE_SIZE_1KBYTES  (0x04U << DCMIPP_IPGR1_MEMORYPAGE_Pos) /*!< Memory Page size 1 Bytes   */
#define DCMIPP_MEMORY_PAGE_SIZE_2KBYTES  (0x05U << DCMIPP_IPGR1_MEMORYPAGE_Pos) /*!< Memory Page size 2 Bytes   */
#define DCMIPP_MEMORY_PAGE_SIZE_4KBYTES  (0x06U << DCMIPP_IPGR1_MEMORYPAGE_Pos) /*!< Memory Page size 4 Bytes   */
#define DCMIPP_MEMORY_PAGE_SIZE_8KBYTES  (0x07U << DCMIPP_IPGR1_MEMORYPAGE_Pos) /*!< Memory Page size 8 Bytes   */
/**
  * @}
  */
/** @defgroup DCMIPP_Maximum_Outstanding_Transactions DCMIPP Maximum Outstanding Transactions
  * @{
  */
#define DCMIPP_OUTSTANDING_TRANSACTION_NONE 0U    /*!< Nooutstanding transaction limitation*/
#define DCMIPP_OUTSTANDING_TRANSACTION_2    0x01U /*!< Two outstanding transactions        */
#define DCMIPP_OUTSTANDING_TRANSACTION_3    0x02U /*!< Three outstanding transactions      */
#define DCMIPP_OUTSTANDING_TRANSACTION_4    0x03U /*!< Four outstanding transactions       */
#define DCMIPP_OUTSTANDING_TRANSACTION_5    0x04U /*!< 5 outstanding transactions          */
#define DCMIPP_OUTSTANDING_TRANSACTION_6    0x05U /*!< 6 outstanding transactions          */
#define DCMIPP_OUTSTANDING_TRANSACTION_7    0x06U /*!< 7 outstanding transactions          */
#define DCMIPP_OUTSTANDING_TRANSACTION_8    0x07U /*!< 8 outstanding transactions          */
#define DCMIPP_OUTSTANDING_TRANSACTION_9    0x08U /*!< 9 outstanding transactions          */
#define DCMIPP_OUTSTANDING_TRANSACTION_10   0x09U /*!< 10 outstanding transactions         */
#define DCMIPP_OUTSTANDING_TRANSACTION_11   0x0AU /*!< 11 outstanding transactions         */
#define DCMIPP_OUTSTANDING_TRANSACTION_12   0x0BU /*!< 12 outstanding transactions         */
#define DCMIPP_OUTSTANDING_TRANSACTION_13   0x0CU /*!< 13 outstanding transactions         */
#define DCMIPP_OUTSTANDING_TRANSACTION_14   0x0DU /*!< 14 outstanding transactions         */
#define DCMIPP_OUTSTANDING_TRANSACTION_15   0x0EU /*!< 15 outstanding transactions         */
#define DCMIPP_OUTSTANDING_TRANSACTION_16   0x0FU /*!< 16 outstanding transactions         */
/**
  * @}
  */

/** @defgroup DCMIPP_Frame_Rates  DCMIPP Frame Rates
  * @{
  */
#define DCMIPP_FRAME_RATE_ALL       0U                               /*!< All frames captured     */
#define DCMIPP_FRAME_RATE_1_OVER_2  (1U << DCMIPP_P0FCTCR_FRATE_Pos) /*!< 1 frame over 2 captured */
#define DCMIPP_FRAME_RATE_1_OVER_4  (2U << DCMIPP_P0FCTCR_FRATE_Pos) /*!< 1 frame over 4 captured */
#define DCMIPP_FRAME_RATE_1_OVER_8  (3U << DCMIPP_P0FCTCR_FRATE_Pos) /*!< 1 frame over 8 captured */
/**
  * @}
  */

/** @defgroup DCMIPP_Crop_Area  DCMIPP Crop Area
  * @{
  */
#define DCMIPP_POSITIVE_AREA  0U                    /*!< Positive Area chosen for crop */
#define DCMIPP_NEGATIVE_AREA  DCMIPP_P0SCSZR_POSNEG /*!< Negative Area chosen for crop */
/**
  * @}
  */

/** @defgroup DCMIPP_Virtual_Channel DCMIPP Virtual Channel
  * @{
  */
#define DCMIPP_VIRTUAL_CHANNEL0   0U /*!< DCMIPP Virtual Channel 0 */
#define DCMIPP_VIRTUAL_CHANNEL1   1U /*!< DCMIPP Virtual Channel 1 */
#define DCMIPP_VIRTUAL_CHANNEL2   2U /*!< DCMIPP Virtual Channel 2 */
#define DCMIPP_VIRTUAL_CHANNEL3   3U /*!< DCMIPP Virtual Channel 3 */
/**
  * @}
  */

/** @defgroup DCMIPP_DataTypeMode DCMIPP DCMIPP Data Type Mode
  * @{
  */
#define DCMIPP_DTMODE_DTIDA                (0U << DCMIPP_P0FSCR_DTMODE_Pos) /*!< Only flow DTIDA from the selected VC
                                                                                 is forwarded in the pipe */
#define DCMIPP_DTMODE_DTIDA_OR_DTIDB       (1U << DCMIPP_P0FSCR_DTMODE_Pos) /*!< Flows DTIDA and/or DTIDB from the
                                                                                selected VC are forwarded in the pipe */
#define DCMIPP_DTMODE_ALL_EXCEPT_DTIA_DTIB (2U << DCMIPP_P0FSCR_DTMODE_Pos) /*!< All data types from the selected VC
                                                                                (except the DTIDA or DTIDB) are
                                                                                 forwarded in the pipe,
                                                                                 only for Pipe0 */
#define DCMIPP_DTMODE_ALL                  (3U << DCMIPP_P0FSCR_DTMODE_Pos) /*!< All data types of the selected virtual
                                                                                 channel VC are forwarded in the pipe,
                                                                                 only for Pipe0 */
/**
  * @}
  */

/** @defgroup DCMIPP_DataTypeSelection DCMIPP Data Type Selection
  * @{
  */
#define DCMIPP_DTSELECT_IDA  DCMIPP_P0FSCR_DTIDA_Pos  /*!< Data type selection ID A */
#define DCMIPP_DTSELECT_IDB  DCMIPP_P0FSCR_DTIDB_Pos  /*!< Data type selection ID B */

/**
  * @}
  */

/** @defgroup DCMIPP_DataType DCMIPP  Data Type
  * @{
  */
#define DCMIPP_DT_YUV420_8   0x18U   /*!< DCMIPP Data Type YUV420 8bit  */
#define DCMIPP_DT_YUV420_10  0x19U   /*!< DCMIPP Data Type YUV420 8bit  */
#define DCMIPP_DT_YUV422_8   0x1EU   /*!< DCMIPP Data Type YUV422 8bit  */
#define DCMIPP_DT_YUV422_10  0x1FU   /*!< DCMIPP Data Type YUV422 10bit */
#define DCMIPP_DT_RGB444     0x20U   /*!< DCMIPP Data Type RGB444       */
#define DCMIPP_DT_RGB555     0x21U   /*!< DCMIPP Data Type RGB555       */
#define DCMIPP_DT_RGB565     0x22U   /*!< DCMIPP Data Type RGB565       */
#define DCMIPP_DT_RGB666     0x23U   /*!< DCMIPP Data Type RGB666       */
#define DCMIPP_DT_RGB888     0x24U   /*!< DCMIPP Data Type RGB888       */
#define DCMIPP_DT_RAW8       0x2AU   /*!< DCMIPP Data Type RawBayer8    */
#define DCMIPP_DT_RAW10      0x2BU   /*!< DCMIPP Data Type RawBayer10   */
#define DCMIPP_DT_RAW12      0x2CU   /*!< DCMIPP Data Type RawBayer12   */
#define DCMIPP_DT_RAW14      0x2DU   /*!< DCMIPP Data Type RawBayer14   */
/**
  * @}
  */
/** @defgroup DCMIPP_CSI_DataTypeID DCMIPP CSI Data Type ID
  * @{
  */
#define DCMIPP_CSI_DT1 1U  /*!< DCMIPP Data Type ID 1 */
#define DCMIPP_CSI_DT2 2U  /*!< DCMIPP Data Type ID 2 */
#define DCMIPP_CSI_DT3 3U  /*!< DCMIPP Data Type ID 3 */
#define DCMIPP_CSI_DT4 4U  /*!< DCMIPP Data Type ID 4 */
#define DCMIPP_CSI_DT5 5U  /*!< DCMIPP Data Type ID 5 */
#define DCMIPP_CSI_DT6 6U  /*!< DCMIPP Data Type ID 6 */
#define DCMIPP_CSI_DT7 7U  /*!< DCMIPP Data Type ID 7 */
/**
  * @}
  */

/** @defgroup DCMIPP_CSI_Number_Of_Lanes DCMIPP CSI Number Of Lanes
  * @{
  */
#define DCMIPP_CSI_ONE_DATA_LANE  (1U << CSI_LMCFGR_LANENB_Pos) /*!< DCMIPP CSI One Data Lane */
#define DCMIPP_CSI_TWO_DATA_LANES (2U << CSI_LMCFGR_LANENB_Pos) /*!< DCMIPP CSI 2 Data Lanes  */
/**
  * @}
  */
/** @defgroup DCMIPP_CSI_DataLaneMapping DCMIPP CSI Data Lane Mapping
  * @{
  */
#define DCMIPP_CSI_PHYSICAL_DATA_LANES   1U  /*!< Physical data lane 0 connected to logical data lane 0 and
                                                  Physical data lane 1 connected to logical data lane 1  */
#define DCMIPP_CSI_INVERTED_DATA_LANES   2U  /*!< Physical data lane 1 connected to logical data lane 0 and
                                                  Physical data lane 0 connected to logical data lane 1 */
/**
  * @}
  */

/** @defgroup DCMIPP_CSI_DataLane DCMIPP CSI Data Lane
  * @{
  */
#define DCMIPP_CSI_DATA_LANE0   1U /*!< DCMIPP CSI Data Lane 0 */
#define DCMIPP_CSI_DATA_LANE1   2U /*!< DCMIPP CSI Data Lane 1 */
/**
  * @}
  */
/** @defgroup DCMIPP_CSI_Timer_StartPoint DCMIPP CSI Timer StartPoint
  * @{
  */
#define DCMIPP_CSI_TIMER_START_SOF  0U /*!< DCMIPP CSI Start timer counter at Start Of Frame */
#define DCMIPP_CSI_TIMER_START_EOF  1U /*!< DCMIPP CSI Start timer counter at End Of Frame   */
/**
  * @}
  */
/** @defgroup DCMIPP_CSI_DataTypeFormat DCMIPP CSI Data Type Format
  * @{
  */
#define DCMIPP_CSI_DT_BPP6  0U  /*!< DCMIPP CSI Data Type Format 6 bit words */
#define DCMIPP_CSI_DT_BPP7  1U  /*!< DCMIPP CSI Data Type Format 7 bit words */
#define DCMIPP_CSI_DT_BPP8  2U  /*!< DCMIPP CSI Data Type Format 8 bit words */
#define DCMIPP_CSI_DT_BPP10 3U  /*!< DCMIPP CSI Data Type Format 10 bit words */
#define DCMIPP_CSI_DT_BPP12 4U  /*!< DCMIPP CSI Data Type Format 12 bit words */
#define DCMIPP_CSI_DT_BPP14 5U  /*!< DCMIPP CSI Data Type Format 14 bit words */
#define DCMIPP_CSI_DT_BPP16 6U  /*!< DCMIPP CSI Data Type Format 16 bit words */
/**
  * @}
  */
/** @defgroup DCMIPP_CSI_Timer DCMIPP CSI Timer
  * @{
  */
#define DCMIPP_CSI_TIMER0 0U     /*!< DCMIPP CSI Timer0 */
#define DCMIPP_CSI_TIMER1 1U  /*!< DCMIPP CSI Timer1 */
#define DCMIPP_CSI_TIMER2 2U  /*!< DCMIPP CSI Timer2 */
#define DCMIPP_CSI_TIMER3 3U  /*!< DCMIPP CSI Timer3 */
/**
  * @}
  */
/** @defgroup DCMIPP_CSI_Counter DCMIPP CSI Counter
  * @{
  */
#define DCMIPP_CSI_COUNTER0 0U /*!< DCMIPP CSI Counter 0 */
#define DCMIPP_CSI_COUNTER1 1U /*!< DCMIPP CSI Counter 1 */
#define DCMIPP_CSI_COUNTER2 2U /*!< DCMIPP CSI Counter 2 */
#define DCMIPP_CSI_COUNTER3 3U /*!< DCMIPP CSI Counter 3 */
/**
  * @}
  */
/**
  * @defgroup DCMIPP_CSI_PHYBitRate DCMIPP CSI PHY BitRate
  * @{
  */
#define DCMIPP_CSI_PHY_BT_80      0U     /*!< PHY BitRate 80      */
#define DCMIPP_CSI_PHY_BT_90      1U     /*!< PHY BitRate 90      */
#define DCMIPP_CSI_PHY_BT_100     2U     /*!< PHY BitRate 100     */
#define DCMIPP_CSI_PHY_BT_110     3U     /*!< PHY BitRate 110     */
#define DCMIPP_CSI_PHY_BT_120     4U     /*!< PHY BitRate 120     */
#define DCMIPP_CSI_PHY_BT_130     5U     /*!< PHY BitRate 130     */
#define DCMIPP_CSI_PHY_BT_140     6U     /*!< PHY BitRate 140     */
#define DCMIPP_CSI_PHY_BT_150     7U     /*!< PHY BitRate 150     */
#define DCMIPP_CSI_PHY_BT_160     8U     /*!< PHY BitRate 160     */
#define DCMIPP_CSI_PHY_BT_170     9U     /*!< PHY BitRate 170     */
#define DCMIPP_CSI_PHY_BT_180     10U    /*!< PHY BitRate 180     */
#define DCMIPP_CSI_PHY_BT_190     11U    /*!< PHY BitRate 190     */
#define DCMIPP_CSI_PHY_BT_205     12U    /*!< PHY BitRate 205     */
#define DCMIPP_CSI_PHY_BT_220     13U    /*!< PHY BitRate 220     */
#define DCMIPP_CSI_PHY_BT_235     14U    /*!< PHY BitRate 235     */
#define DCMIPP_CSI_PHY_BT_250     15U    /*!< PHY BitRate 250     */
#define DCMIPP_CSI_PHY_BT_275     16U    /*!< PHY BitRate 275     */
#define DCMIPP_CSI_PHY_BT_300     17U    /*!< PHY BitRate 300     */
#define DCMIPP_CSI_PHY_BT_325     18U    /*!< PHY BitRate 325     */
#define DCMIPP_CSI_PHY_BT_350     19U    /*!< PHY BitRate 350     */
#define DCMIPP_CSI_PHY_BT_400     20U    /*!< PHY BitRate 400     */
#define DCMIPP_CSI_PHY_BT_450     21U    /*!< PHY BitRate 450     */
#define DCMIPP_CSI_PHY_BT_500     22U    /*!< PHY BitRate 500     */
#define DCMIPP_CSI_PHY_BT_550     23U    /*!< PHY BitRate 550     */
#define DCMIPP_CSI_PHY_BT_600     24U    /*!< PHY BitRate 600     */
#define DCMIPP_CSI_PHY_BT_650     25U    /*!< PHY BitRate 650     */
#define DCMIPP_CSI_PHY_BT_700     26U    /*!< PHY BitRate 700     */
#define DCMIPP_CSI_PHY_BT_750     27U    /*!< PHY BitRate 750     */
#define DCMIPP_CSI_PHY_BT_800     28U    /*!< PHY BitRate 800     */
#define DCMIPP_CSI_PHY_BT_850     29U    /*!< PHY BitRate 850     */
#define DCMIPP_CSI_PHY_BT_900     30U    /*!< PHY BitRate 900     */
#define DCMIPP_CSI_PHY_BT_950     31U    /*!< PHY BitRate 950     */
#define DCMIPP_CSI_PHY_BT_1000    32U    /*!< PHY BitRate 1000    */
#define DCMIPP_CSI_PHY_BT_1050    33U    /*!< PHY BitRate 1050    */
#define DCMIPP_CSI_PHY_BT_1100    34U    /*!< PHY BitRate 1100    */
#define DCMIPP_CSI_PHY_BT_1150    35U    /*!< PHY BitRate 1150    */
#define DCMIPP_CSI_PHY_BT_1200    36U    /*!< PHY BitRate 1200    */
#define DCMIPP_CSI_PHY_BT_1250    37U    /*!< PHY BitRate 1250    */
#define DCMIPP_CSI_PHY_BT_1300    38U    /*!< PHY BitRate 1300    */
#define DCMIPP_CSI_PHY_BT_1350    39U    /*!< PHY BitRate 1350    */
#define DCMIPP_CSI_PHY_BT_1400    40U    /*!< PHY BitRate 1400    */
#define DCMIPP_CSI_PHY_BT_1450    41U    /*!< PHY BitRate 1450    */
#define DCMIPP_CSI_PHY_BT_1500    42U    /*!< PHY BitRate 1500    */
#define DCMIPP_CSI_PHY_BT_1550    43U    /*!< PHY BitRate 1550    */
#define DCMIPP_CSI_PHY_BT_1600    44U    /*!< PHY BitRate 1600    */
#define DCMIPP_CSI_PHY_BT_1650    45U    /*!< PHY BitRate 1650    */
#define DCMIPP_CSI_PHY_BT_1700    46U    /*!< PHY BitRate 1700    */
#define DCMIPP_CSI_PHY_BT_1750    47U    /*!< PHY BitRate 1750    */
#define DCMIPP_CSI_PHY_BT_1800    48U    /*!< PHY BitRate 1800    */
#define DCMIPP_CSI_PHY_BT_1850    49U    /*!< PHY BitRate 1850    */
#define DCMIPP_CSI_PHY_BT_1900    50U    /*!< PHY BitRate 1900    */
#define DCMIPP_CSI_PHY_BT_1950    51U    /*!< PHY BitRate 1950    */
#define DCMIPP_CSI_PHY_BT_2000    52U    /*!< PHY BitRate 2000    */
#define DCMIPP_CSI_PHY_BT_2050    53U    /*!< PHY BitRate 2050    */
#define DCMIPP_CSI_PHY_BT_2100    54U    /*!< PHY BitRate 2100    */
#define DCMIPP_CSI_PHY_BT_2150    55U    /*!< PHY BitRate 2150    */
#define DCMIPP_CSI_PHY_BT_2200    56U    /*!< PHY BitRate 2200    */
#define DCMIPP_CSI_PHY_BT_2250    57U    /*!< PHY BitRate 2250    */
#define DCMIPP_CSI_PHY_BT_2300    58U    /*!< PHY BitRate 2300    */
#define DCMIPP_CSI_PHY_BT_2350    59U    /*!< PHY BitRate 2350    */
#define DCMIPP_CSI_PHY_BT_2400    60U    /*!< PHY BitRate 2400    */
#define DCMIPP_CSI_PHY_BT_2450    61U    /*!< PHY BitRate 2450    */
#define DCMIPP_CSI_PHY_BT_2500    62U    /*!< PHY BitRate 2500    */
/**
  * @}
  */

/** @defgroup DCMIPP_OutputSamplesType  DCMIPP OutputSamplesType
  * @{
  */
#define DCMIPP_CLAMP_YUV (0x0U << DCMIPP_P1CCCR_TYPE_Pos) /*!< Output samples type : Clamped to [16;235] for Y and
                                                               to [16;240] for U and V  */
#define DCMIPP_CLAMP_RGB (0x1U << DCMIPP_P1CCCR_TYPE_Pos) /*!< Output samples type:Clamped to [16;235] for R, G and B */
/**
  * @}
  */
/** @defgroup DCMIPP_Format  DCMIPP Format
  * @{
  */
#define  DCMIPP_FORMAT_BYTE             0U                               /*!< DCMIPP Format BYTE    */
#define  DCMIPP_FORMAT_YUV422          (0x1EU << DCMIPP_PRCR_FORMAT_Pos) /*!< DCMIPP Format YUV422  */
#define  DCMIPP_FORMAT_RGB565          (0x22U << DCMIPP_PRCR_FORMAT_Pos) /*!< DCMIPP Format RGB565  */
#define  DCMIPP_FORMAT_RGB666          (0x23U << DCMIPP_PRCR_FORMAT_Pos) /*!< DCMIPP Format RGB666  */
#define  DCMIPP_FORMAT_RGB888          (0x24U << DCMIPP_PRCR_FORMAT_Pos) /*!< DCMIPP Format RGB888  */
#define  DCMIPP_FORMAT_RAW8            (0x2AU << DCMIPP_PRCR_FORMAT_Pos) /*!< DCMIPP Format RAW8    */
#define  DCMIPP_FORMAT_RAW10           (0x2BU << DCMIPP_PRCR_FORMAT_Pos) /*!< DCMIPP Format RAW10   */
#define  DCMIPP_FORMAT_RAW12           (0x2CU << DCMIPP_PRCR_FORMAT_Pos) /*!< DCMIPP Format RAW12   */
#define  DCMIPP_FORMAT_RAW14           (0x2DU << DCMIPP_PRCR_FORMAT_Pos) /*!< DCMIPP Format RAW14   */
#define  DCMIPP_FORMAT_MONOCHROME_8B   (0x4AU << DCMIPP_PRCR_FORMAT_Pos) /*!< DCMIPP Format 8-bits  */
#define  DCMIPP_FORMAT_MONOCHROME_10B  (0x4BU << DCMIPP_PRCR_FORMAT_Pos) /*!< DCMIPP Format 10-bits */
#define  DCMIPP_FORMAT_MONOCHROME_12B  (0x4CU << DCMIPP_PRCR_FORMAT_Pos) /*!< DCMIPP Format 12-bits */
#define  DCMIPP_FORMAT_MONOCHROME_14B  (0x4DU << DCMIPP_PRCR_FORMAT_Pos) /*!< DCMIPP Format 14-bits */
/**
  * @}
  */

/** @defgroup DCMIPP_Extended_Data_Mode  DCMIPP Extended Data Mode
  * @{
  */
#define  DCMIPP_INTERFACE_8BITS   0U                         /*!< Interface captures 8bits on every pixel clock  */
#define  DCMIPP_INTERFACE_10BITS (1U << DCMIPP_PRCR_EDM_Pos) /*!< Interface captures 10bits on every pixel clock */
#define  DCMIPP_INTERFACE_12BITS (2U << DCMIPP_PRCR_EDM_Pos) /*!< Interface captures 12bits on every pixel clock */
#define  DCMIPP_INTERFACE_14BITS (3U << DCMIPP_PRCR_EDM_Pos) /*!< Interface captures 14bits on every pixel clock */
#define  DCMIPP_INTERFACE_16BITS (4U << DCMIPP_PRCR_EDM_Pos) /*!< Interface captures 16bits on every pixel clock */
/**
  * @}
  */

/** @defgroup DCMIPP_HSYNC_Polarity DCMIPP HSYNC Polarity
  * @{
  */
#define DCMIPP_HSPOLARITY_LOW       0U                 /*!< Horizontal synchronization active Low  */
#define DCMIPP_HSPOLARITY_HIGH      DCMIPP_PRCR_HSPOL  /*!< Horizontal synchronization active High */
/**
  * @}
  */
/** @defgroup DCMIPP_VSYNC_Polarity DCMIPP VSYNC Polarity
  * @{
  */
#define DCMIPP_VSPOLARITY_LOW       0U                 /*!< Vertical synchronization active Low  */
#define DCMIPP_VSPOLARITY_HIGH      DCMIPP_PRCR_VSPOL  /*!< Vertical synchronization active High */
/**
  * @}
  */
/** @defgroup DCMIPP_PIXCK_Polarity DCMIPP PIXCK Polarity
  * @{
  */
#define DCMIPP_PCKPOLARITY_FALLING  0U                  /*!< Pixel clock active on Falling edge */
#define DCMIPP_PCKPOLARITY_RISING   DCMIPP_PRCR_PCKPOL  /*!< Pixel clock active on Rising edge  */
/**
  * @}
  */

/** @defgroup DCMIPP_Synchronization_Mode DCMIPP Synchronization Mode
  * @{
  */
#define DCMIPP_SYNCHRO_HARDWARE     0U                          /*!< Hardware Synchronization */
#define DCMIPP_SYNCHRO_EMBEDDED    DCMIPP_PRCR_ESS              /*!< Embedded Synchronization */
/**
  * @}
  */
/** @defgroup DCMIPP_SWAP_COMPONENT  DCMIPP Swap Component
  * @{
  */
#define DCMIPP_SWAP_COMPONENT_DISABLE  0U                   /*!< Disable Swap R/U and B/V */
#define DCMIPP_SWAP_COMPONENT_ENABLE   (DCMIPP_CMCR_SWAPRB) /*!< Enable Swap R/U and B/V  */
/**
  * @}
  */

/** @defgroup DCMIPP_SWAP_CYCLES  DCMIPP Swap Cycles
  * @{
  */
#define DCMIPP_SWAPCYCLES_DISABLE  0U                       /*!< swap data from cycle 0 vs cycle 1 */
#define DCMIPP_SWAPCYCLES_ENABLE   (DCMIPP_PRCR_SWAPCYCLES) /*!< swap data from cycle 0 vs cycle 1 */
/**
  * @}
  */

/** @defgroup DCMIPP_SWAP_BITS  DCMIPP Swap Bits
  * @{
  */
#define DCMIPP_SWAPBITS_DISABLE  0U                     /*!< swap lsb vs msb within each received component */
#define DCMIPP_SWAPBITS_ENABLE   (DCMIPP_PRCR_SWAPBITS) /*!< swap lsb vs msb within each received component */
/**
  * @}
  */

/** @defgroup DCMIPP_Pipe_Line_Decimation DCMIPP Pipe Line Decimation
  * @{
  */
/** @defgroup DCMIPP_Line_Select_Mode DCMIPP Line Select Mode
  * @{
  */
#define DCMIPP_LSM_ALL          0U                              /*!< Interface captures all received lines */
#define DCMIPP_LSM_ALTERNATE_2 (1U << DCMIPP_P0PPCR_LSM_Pos )   /*!< Interface captures one line out of two */
/**
  * @}
  */
/** @defgroup DCMIPP_Line_Start_Mode DCMIPP Line Start Mode
  * @{
  */
#define DCMIPP_OELS_ODD         0U                              /*!< Interface captures first line from the frame start,
                                                                     second one is dropped */
#define DCMIPP_OELS_EVEN       (1U << DCMIPP_P0PPCR_OELS_Pos)   /*!< Interface captures second line from the frame
                                                                     start, first one is dropped */
/**
  * @}
  */
/**
  * @}
  */

/** @defgroup DCMIPP_Pipe_Byte_Decimation DCMIPP Pipe Byte Decimation
  * @{
  */
/** @defgroup DCMIPP_Byte_Select_Mode DCMIPP Byte Select Mode
  * @{
  */
#define DCMIPP_BSM_ALL          0U                              /*!< Interface captures all received data */
#define DCMIPP_BSM_DATA_OUT_2  (1U << DCMIPP_P0PPCR_BSM_Pos)    /*!< Interface captures 1 data out of 2   */
#define DCMIPP_BSM_BYTE_OUT_4  (2U << DCMIPP_P0PPCR_BSM_Pos)    /*!< Interface captures 1 byte out of 4   */
#define DCMIPP_BSM_2BYTE_OUT_4 (3U << DCMIPP_P0PPCR_BSM_Pos)    /*!< Interface captures 2 byte out of 4   */
/**
  * @}
  */
/** @defgroup DCMIPP_Byte_Start_Mode DCMIPP Byte Start Mode
  * @{
  */
#define DCMIPP_OEBS_ODD        0U                              /*!< Interface captures first data (byte or double byte)
                                                                   from the frame/line start,second one being dropped */
#define DCMIPP_OEBS_EVEN      (1U << DCMIPP_P0PPCR_OEBS_Pos)   /*!< Interface captures second data (byte or double byte)
                                                                    from the frame/line start, first one is dropped   */
/**
  * @}
  */
/**
  * @}
  */

/** @defgroup DCMIPP_Memory DCMIPP Memory
  * @{
  */
#define DCMIPP_MEMORY_ADDRESS_0 0U   /*!< Base destination address   */
#define DCMIPP_MEMORY_ADDRESS_1 1U   /*!< Second destination address */
/**
  * @}
  */
/** @defgroup DCMIPP_LineMult DCMIPP Line Mult
  * @{
  */
#define  DCMIPP_MULTILINE_1_LINE      0U                                  /*!< Event after every 1   line   */
#define  DCMIPP_MULTILINE_2_LINES    (1U << DCMIPP_P0PPCR_LINEMULT_Pos)   /*!< Event after every 2   lines  */
#define  DCMIPP_MULTILINE_4_LINES    (2U << DCMIPP_P0PPCR_LINEMULT_Pos)   /*!< Event after every 4   lines  */
#define  DCMIPP_MULTILINE_8_LINES    (3U << DCMIPP_P0PPCR_LINEMULT_Pos)   /*!< Event after every 8   lines  */
#define  DCMIPP_MULTILINE_16_LINES   (4U << DCMIPP_P0PPCR_LINEMULT_Pos)   /*!< Event after every 16  lines  */
#define  DCMIPP_MULTILINE_32_LINES   (5U << DCMIPP_P0PPCR_LINEMULT_Pos)   /*!< Event after every 32  lines  */
#define  DCMIPP_MULTILINE_64_LINES   (6U << DCMIPP_P0PPCR_LINEMULT_Pos)   /*!< Event after every 64  lines  */
#define  DCMIPP_MULTILINE_128_LINES  (7U << DCMIPP_P0PPCR_LINEMULT_Pos)   /*!< Event after every 128 lines  */
/**
  * @}
  */
/** @defgroup DCMIPP_LineWrapAddress DCMIPP line Wrap Address
  * @{
  */
#define  DCMIPP_WRAP_ADDRESS_1_LINE     (0U << DCMIPP_P1PPCR_LMAWM_Pos)  /*!< Wraps address after 1   line   */
#define  DCMIPP_WRAP_ADDRESS_2_LINES    (1U << DCMIPP_P1PPCR_LMAWM_Pos)  /*!< Wraps address after 2   lines  */
#define  DCMIPP_WRAP_ADDRESS_4_LINES    (2U << DCMIPP_P1PPCR_LMAWM_Pos)  /*!< Wraps address after 4   lines  */
#define  DCMIPP_WRAP_ADDRESS_8_LINES    (3U << DCMIPP_P1PPCR_LMAWM_Pos)  /*!< Wraps address after 8   lines  */
#define  DCMIPP_WRAP_ADDRESS_16_LINES   (4U << DCMIPP_P1PPCR_LMAWM_Pos)  /*!< Wraps address after 16  lines  */
#define  DCMIPP_WRAP_ADDRESS_32_LINES   (5U << DCMIPP_P1PPCR_LMAWM_Pos)  /*!< Wraps address after 32  lines  */
#define  DCMIPP_WRAP_ADDRESS_64_LINES   (6U << DCMIPP_P1PPCR_LMAWM_Pos)  /*!< Wraps address after 64  lines  */
#define  DCMIPP_WRAP_ADDRESS_128_LINES  (7U << DCMIPP_P1PPCR_LMAWM_Pos)  /*!< Wraps address after 128 lines  */
/**
  * @}
  */

/** @defgroup DCMIPP_PIPE_Vertical_Decimation_Ratio DCMIPP Pipe Vertical Decimation Ratio
  * @{
  */

#define  DCMIPP_VDEC_ALL       0U                              /*!< All lines captured , no vertical decimation */
#define  DCMIPP_VDEC_1_OUT_2  (1U << DCMIPP_P1DECR_VDEC_Pos)   /*!< One line out of two transmitted             */
#define  DCMIPP_VDEC_1_OUT_4  (2U << DCMIPP_P1DECR_VDEC_Pos)   /*!< One line out of four transmitted            */
#define  DCMIPP_VDEC_1_OUT_8  (3U << DCMIPP_P1DECR_VDEC_Pos)   /*!< One line out of eight transmitted           */

/**
  * @}
  */

/** @defgroup DCMIPP_PIPE_Horizontal_Decimation_Ratio DCMIPP Pipe Horizontal Decimation Ratio
  * @{
  */
#define  DCMIPP_HDEC_ALL       0U                              /*!< All pixels captured , no horizontal decimation */
#define  DCMIPP_HDEC_1_OUT_2  (1U << DCMIPP_P1DECR_HDEC_Pos)   /*!< One line out of two transmitted                */
#define  DCMIPP_HDEC_1_OUT_4  (2U << DCMIPP_P1DECR_HDEC_Pos)   /*!< One line out of four transmitted               */
#define  DCMIPP_HDEC_1_OUT_8  (3U << DCMIPP_P1DECR_HDEC_Pos)   /*!< One line out of eight transmitted              */
/**
  * @}
  */

/** @defgroup DCMIPP_Pixel_Packer_Format  DCMIPP Pixel Packer Format
  * @{
  */
#define  DCMIPP_PIXEL_PACKER_FORMAT_RGB888_YUV444_1  0U                                /*!< RGB888 or YUV422 1-buffer*/
#define  DCMIPP_PIXEL_PACKER_FORMAT_RGB565_1        (1U << DCMIPP_P1PPCR_FORMAT_Pos)   /*!< RGB565 1-buffer          */
#define  DCMIPP_PIXEL_PACKER_FORMAT_ARGB8888        (2U << DCMIPP_P1PPCR_FORMAT_Pos)   /*!< ARGB888  A=0xff          */
#define  DCMIPP_PIXEL_PACKER_FORMAT_RGBA888         (3U << DCMIPP_P1PPCR_FORMAT_Pos)   /*!< RGBA888  A=0xff)         */
#define  DCMIPP_PIXEL_PACKER_FORMAT_MONO_Y8_G8_1    (4U << DCMIPP_P1PPCR_FORMAT_Pos)   /*!< Monochrome Y8 or G8      */
#define  DCMIPP_PIXEL_PACKER_FORMAT_YUV444_1        (5U << DCMIPP_P1PPCR_FORMAT_Pos)   /*!< YUV444 1-buffer 32bpp
                                                                                            A=0xff)                  */
#define  DCMIPP_PIXEL_PACKER_FORMAT_YUV422_1        (6U << DCMIPP_P1PPCR_FORMAT_Pos)   /*!< YUV422 1-buffer 16bpp    */
#define  DCMIPP_PIXEL_PACKER_FORMAT_YUV422_2        (7U << DCMIPP_P1PPCR_FORMAT_Pos)   /*!< YUV422 2-buffer 16bpp    */
#define  DCMIPP_PIXEL_PACKER_FORMAT_YUV420_2        (8U << DCMIPP_P1PPCR_FORMAT_Pos)   /*!< YUV420 2-buffer 12bpp    */
#define  DCMIPP_PIXEL_PACKER_FORMAT_YUV420_3        (9U << DCMIPP_P1PPCR_FORMAT_Pos)   /*!< YUV420 3-buffer 12bpp    */
/**
  * @}
  */

/** @defgroup DCMIPP_Statistics_Extraction_Mode  DCMIPP Statistics extraction Mode
  * @{
  */
#define  DCMIPP_STAT_EXT_MODE_AVERAGE   0U                               /*!< Values of pixels are accumulated as-is */
#define  DCMIPP_STAT_EXT_MODE_BINS     (1U << DCMIPP_P1ST1CR_MODE_Pos)   /*!< Values of pixels are used to see
                                                                              if the pixel fits one of the 12 bins.*/
/**
  * @}
  */

/** @defgroup DCMIPP_Statistics_Extraction_Module_ID  DCMIPP Statistics Extraction Module ID
  * @{
  */
#define DCMIPP_STATEXT_MODULE1  1U /*!< Statistic Extraction Module 1 */
#define DCMIPP_STATEXT_MODULE2  2U /*!< Statistic Extraction Module 2 */
#define DCMIPP_STATEXT_MODULE3  3U /*!< Statistic Extraction Module 3 */
/**
  * @}
  */

/** @defgroup DCMIPP_Statistics_Extraction_Source  DCMIPP Statistics extraction Source
  * @{
  */
#define  DCMIPP_STAT_EXT_SOURCE_PRE_BLKLVL_R   0U                              /*!< Statistics Extraction Source : Red
                                                           is sampled before the black level calibration and exposure */
#define  DCMIPP_STAT_EXT_SOURCE_PRE_BLKLVL_G  (1U << DCMIPP_P1ST1CR_SRC_Pos) /*!< Statistics Extraction Source : Green
                                                           is sampled before the black level calibration and exposure */
#define  DCMIPP_STAT_EXT_SOURCE_PRE_BLKLVL_B  (2U << DCMIPP_P1ST1CR_SRC_Pos) /*!< Statistics Extraction Source : Blue
                                                           is sampled before the black level calibration and exposure */
#define  DCMIPP_STAT_EXT_SOURCE_PRE_BLKLVL_L  (3U << DCMIPP_P1ST1CR_SRC_Pos) /*!< Statistics Extraction Source :
                                                 Luminance is sampled before the black level calibration and exposure */
#define  DCMIPP_STAT_EXT_SOURCE_POST_DEMOS_R  (4U << DCMIPP_P1ST1CR_SRC_Pos) /*!< Statistics Extraction Source : Red
                                                            is sampled after the black level calibration and exposure */
#define  DCMIPP_STAT_EXT_SOURCE_POST_DEMOS_G  (5U << DCMIPP_P1ST1CR_SRC_Pos) /*!< Statistics Extraction Source : Green
                                                            is sampled after the black level calibration and exposure */
#define  DCMIPP_STAT_EXT_SOURCE_POST_DEMOS_B  (6U << DCMIPP_P1ST1CR_SRC_Pos) /*!< Statistics Extraction Source : Blue
                                                            is sampled after the black level calibration and exposure */
#define  DCMIPP_STAT_EXT_SOURCE_POST_DEMOS_L  (7U << DCMIPP_P1ST1CR_SRC_Pos) /*!< Statistics Extraction Source :
                                                  Luminance is sampled after the black level calibration and exposure */
/**
  * @}
  */

/** @defgroup DCMIPP_Statistics_Extraction_Bins  DCMIPP Statistics extraction Bins
  * @{
  */
#define  DCMIPP_STAT_EXT_BINS_MODE_LOWER_BINS   0U                               /*!< LowerBins: Accu0 (1, 2)
                                                                                      is incremented of 256 if
                                                                                      Component <4 (<8, <16) */
#define  DCMIPP_STAT_EXT_BINS_MODE_LOWMID_BINS (1U << DCMIPP_P1ST1CR_BINS_Pos) /*!< LowMid Bins: Accu0 (1, 2)
                                                                                      is incremented of 256 if
                                                                                      Component <32 (<64, <128) */
#define  DCMIPP_STAT_EXT_BINS_MODE_UPMID_BINS  (2U << DCMIPP_P1ST1CR_BINS_Pos) /*!< UpMid Bins: Accu0 (1, 2) is
                                                                                      incremented of 256 if
                                                                                      Component >127 (>191, >224) */
#define  DCMIPP_STAT_EXT_BINS_MODE_UP_BINS     (3U << DCMIPP_P1ST1CR_BINS_Pos) /*!< UpBins: Accu0 (1, 2) is
                                                                                      incremented of 256 if
                                                                                      Component >239 (>247, >251)
                                                                                      Condition: MODE = Average */
#define  DCMIPP_STAT_EXT_AVER_MODE_ALL_PIXELS  (0U << DCMIPP_P1ST1CR_BINS_Pos) /*!< All Pixels: Accu is incremented of
                                                                                   Component, if 0 <= Component < 256 */
#define  DCMIPP_STAT_EXT_AVER_MODE_NOEXT16     (1U << DCMIPP_P1ST1CR_BINS_Pos) /*!< NoExt16: Accu is incremented of
                                                                                  Component, if 16 <= Component < 240 */
#define  DCMIPP_STAT_EXT_AVER_MODE_NOEXT32     (2U << DCMIPP_P1ST1CR_BINS_Pos) /*!< NoExt32: Accu is incremented of
                                                                                  Component, if 32 <= Component < 224 */
#define  DCMIPP_STAT_EXT_AVER_MODE_NOEXT64     (3U << DCMIPP_P1ST1CR_BINS_Pos) /*!< NoExt64: Accu is incremented of
                                                                                  Component, if 64 <= Component < 192 */
/**
  * @}
  */

/**
  * @defgroup  DCMIPP_RAWBayer2RGB_Strength DCMIPP RAWBayer2RGB Strength
  * @{
  */
#define  DCMIPP_RAWBAYER_ALGO_NONE        0U   /*!< No edge detection, pure linear interpolation */
#define  DCMIPP_RAWBAYER_ALGO_STRENGTH_3  1U   /*!< RawBayer relative algorithm strength 3       */
#define  DCMIPP_RAWBAYER_ALGO_STRENGTH_4  2U   /*!< RawBayer relative algorithm strength 4       */
#define  DCMIPP_RAWBAYER_ALGO_STRENGTH_6  3U   /*!< RawBayer relative algorithm strength 6       */
#define  DCMIPP_RAWBAYER_ALGO_STRENGTH_8  4U   /*!< RawBayer relative algorithm strength 8       */
#define  DCMIPP_RAWBAYER_ALGO_STRENGTH_12 5U   /*!< RawBayer relative algorithm strength 12      */
#define  DCMIPP_RAWBAYER_ALGO_STRENGTH_16 6U   /*!< RawBayer relative algorithm strength 16      */
#define  DCMIPP_RAWBAYER_ALGO_STRENGTH_24 7U   /*!< RawBayer relative algorithm strength 24      */

/**
  * @}
  */

/**
  * @defgroup  DCMIPP_RAWBayer2RGB_Type DCMIPP RAWBayer2RGB Type
  * @{
  */
#define  DCMIPP_RAWBAYER_RGGB   0U                               /*!<Raw Bayer Type RGGB */
#define  DCMIPP_RAWBAYER_GRBG  (1U << DCMIPP_P1DMCR_TYPE_Pos)    /*!<Raw Bayer Type GRBG */
#define  DCMIPP_RAWBAYER_GBRG  (2U << DCMIPP_P1DMCR_TYPE_Pos)    /*!<Raw Bayer Type GBRG */
#define  DCMIPP_RAWBAYER_BGGR  (3U << DCMIPP_P1DMCR_TYPE_Pos)    /*!<Raw Bayer Type BGGR */
/**
  * @}
  */

/** @defgroup DCMIPP_Bad_Pixel_Removal_Strength  DCMIPP Bad Pixel Removal Strength
  * @{
  */
#define  DCMIPP_BAD_PXL_REM_SRENGTH_0  0U   /*!< The filter is fairly tolerant: only a
                                                  few pixels are declared bad, due to
                                                  their high noise */
#define  DCMIPP_BAD_PXL_REM_SRENGTH_1  1U   /*!< Bad Pixel detection strength 1 */
#define  DCMIPP_BAD_PXL_REM_SRENGTH_2  2U   /*!< Bad Pixel detection strength 2 */
#define  DCMIPP_BAD_PXL_REM_SRENGTH_3  3U   /*!< Bad Pixel detection strength 3 */
#define  DCMIPP_BAD_PXL_REM_SRENGTH_4  4U   /*!< Bad Pixel detection strength 4 */
#define  DCMIPP_BAD_PXL_REM_SRENGTH_5  5U   /*!< Bad Pixel detection strength 5 */
#define  DCMIPP_BAD_PXL_REM_SRENGTH_6  6U   /*!< Bad Pixel detection strength 6 */
#define  DCMIPP_BAD_PXL_REM_SRENGTH_7  7U   /*!< Bad Pixel detection strength 7 */
/**
  * @}
  */

/** @defgroup DCMIPP_Region_Of_Interest  DCMIPP Region Of Interest
  * @{
  */
#define DCMIPP_REGION_OF_INTEREST1  1U  /*!< Region Of Interest 1 */
#define DCMIPP_REGION_OF_INTEREST2  2U  /*!< Region Of Interest 2 */
#define DCMIPP_REGION_OF_INTEREST3  3U  /*!< Region Of Interest 3 */
#define DCMIPP_REGION_OF_INTEREST4  4U  /*!< Region Of Interest 4 */
#define DCMIPP_REGION_OF_INTEREST5  5U  /*!< Region Of Interest 5 */
#define DCMIPP_REGION_OF_INTEREST6  6U  /*!< Region Of Interest 6 */
#define DCMIPP_REGION_OF_INTEREST7  7U  /*!< Region Of Interest 7 */
#define DCMIPP_REGION_OF_INTEREST8  8U  /*!< Region Of Interest 8 */
/**
  * @}
  */

/** @defgroup DCMIPP_Region_Of_Interest_Line_Width  DCMIPP Region Of Interest Line Width
  * @{
  */
#define DCMIPP_LINE_WIDTH_1PXL 0U    /*!< Line width 1 pixel  */
#define DCMIPP_LINE_WIDTH_2PXL 1U    /*!< Line width 2 pixels */
#define DCMIPP_LINE_WIDTH_4PXL 2U    /*!< Line width 4 pixels */
#define DCMIPP_LINE_WIDTH_8PXL 3U    /*!< Line width 8 pixels */
/**
  * @}
  */

/** @defgroup DCMIPP_Multiply  DCMIPP Multiply
  * @{
  */
#define DCMIPP_MULTIPY_BY_1 0U   /*!< DCMIPP multiply R/G/B by 1 */
#define DCMIPP_MULTIPY_BY_2 1U   /*!< DCMIPP multiply R/G/B by 2 */
#define DCMIPP_MULTIPY_BY_4 2U   /*!< DCMIPP multiply R/G/B by 4 */
#define DCMIPP_MULTIPY_BY_8 3U   /*!< DCMIPP multiply R/G/B by 8 */
/**
  * @}
  */

/** @defgroup DCMIPP_Interrupt_Sources  DCMIPP Interrupt sources
  * @{
  */
#define DCMIPP_IT_AXI_TRANSFER_ERROR  DCMIPP_CMIER_ATXERRIE  /*!< IPPLUG AXI Transfer error interrupt        */
#define DCMIPP_IT_PARALLEL_SYNC_ERROR DCMIPP_CMIER_PRERRIE   /*!< Sync error interrupt on parallel interface */
#define DCMIPP_IT_PIPE0_FRAME         DCMIPP_CMIER_P0FRAMEIE /*!< Frame capture interrupt complete for pipe0 */
#define DCMIPP_IT_PIPE0_VSYNC         DCMIPP_CMIER_P0VSYNCIE /*!< Vertical sync interrupt for pipe0          */
#define DCMIPP_IT_PIPE0_LINE          DCMIPP_CMIER_P0LINEIE  /*!< Multiline interrupt for pipe0              */
#define DCMIPP_IT_PIPE0_LIMIT         DCMIPP_CMIER_P0LIMITIE /*!< Limit interrupt for pipe0                  */
#define DCMIPP_IT_PIPE0_OVR           DCMIPP_CMIER_P0OVRIE   /*!< Overrun interrupt for pipe0                */
#define DCMIPP_IT_PIPE1_LINE          DCMIPP_CMIER_P1LINEIE  /*!< Multiline capture interrupt for pipe1      */
#define DCMIPP_IT_PIPE1_FRAME         DCMIPP_CMIER_P1FRAMEIE /*!< Frame capture interrupt complete for pipe1 */
#define DCMIPP_IT_PIPE1_VSYNC         DCMIPP_CMIER_P1VSYNCIE /*!< Vertical sync interrupt for pipe1          */
#define DCMIPP_IT_PIPE1_OVR           DCMIPP_CMIER_P1OVRIE   /*!< Overrun interrupt for pipe1                */
#define DCMIPP_IT_PIPE2_LINE          DCMIPP_CMIER_P2LINEIE  /*!< Multiline capture interrupt for pipe2      */
#define DCMIPP_IT_PIPE2_FRAME         DCMIPP_CMIER_P2FRAMEIE /*!< Frame capture interrupt complete for pipe2 */
#define DCMIPP_IT_PIPE2_VSYNC         DCMIPP_CMIER_P2VSYNCIE /*!< Vertical synch interrupt for pipe2         */
#define DCMIPP_IT_PIPE2_OVR           DCMIPP_CMIER_P2OVRIE   /*!< Overrun interrupt for pipe2                */
/**
  * @}
  */

/** @defgroup DCMIPP_Interrupt_Flags  DCMIPP Interrupt Flags
  * @{
  */
#define DCMIPP_FLAG_AXI_TRANSFER_ERROR  DCMIPP_CMSR2_ATXERRF /*!< IPPLUG AXI Transfer error interrupt flag            */
#define DCMIPP_FLAG_PARALLEL_SYNC_ERROR DCMIPP_CMSR2_PRERRF  /*!< Synchronization error interrupt on parallel interface
                                                                  flag */
#define DCMIPP_FLAG_PIPE0_FRAME       DCMIPP_CMSR2_P0FRAMEF  /*!< Frame capture interrupt complete for pipe0 flag     */
#define DCMIPP_FLAG_PIPE0_VSYNC       DCMIPP_CMSR2_P0VSYNCF  /*!< Vertical synch interrupt for pipe0 flag             */
#define DCMIPP_FLAG_PIPE0_LINE        DCMIPP_CMSR2_P0LINEF   /*!< Multiline interrupt for pipe0 flag                  */
#define DCMIPP_FLAG_PIPE0_LIMIT       DCMIPP_CMSR2_P0LIMITF  /*!< Limit interrupt for pipe0 flag                      */
#define DCMIPP_FLAG_PIPE0_OVR         DCMIPP_CMSR2_P0OVRF    /*!< Overrun interrupt for pipe0 flag                    */
#define DCMIPP_FLAG_PIPE1_LINE        DCMIPP_CMSR2_P1LINEF   /*!< Multiline capture interrupt for pipe1 flag          */
#define DCMIPP_FLAG_PIPE1_FRAME       DCMIPP_CMSR2_P1FRAMEF  /*!< Frame capture interrupt complete for pipe1 flag     */
#define DCMIPP_FLAG_PIPE1_VSYNC       DCMIPP_CMSR2_P1VSYNCF  /*!< Vertical synch interrupt for pipe1 flag             */
#define DCMIPP_FLAG_PIPE1_OVR         DCMIPP_CMSR2_P1OVRF    /*!< Overrun interrupt for pipe1 flag                    */
#define DCMIPP_FLAG_PIPE2_LINE        DCMIPP_CMSR2_P2LINEF   /*!< Multiline capture interrupt for pipe2 flag          */
#define DCMIPP_FLAG_PIPE2_FRAME       DCMIPP_CMSR2_P2FRAMEF  /*!< Frame capture interrupt complete for pipe2 flag     */
#define DCMIPP_FLAG_PIPE2_VSYNC       DCMIPP_CMSR2_P2VSYNCF  /*!< Vertical synch interrupt for pipe2 flag             */
#define DCMIPP_FLAG_PIPE2_OVR         DCMIPP_CMSR2_P2OVRF    /*!< Overrun interrupt for pipe2 flag                    */
/**
  * @}
  */
/** @defgroup DCMIPP_CSI_Interrupt_Sources  DCMIPP CSI Interrupt sources
  * @{
  */
#define DCMIPP_CSI_IT_SYNCERR      CSI_IER0_SYNCERRIE     /*!< Synchronization error interrupt                    */
#define DCMIPP_CSI_IT_WDERR        CSI_IER0_WDERRIE       /*!< Watchdog error interrupt                           */
#define DCMIPP_CSI_IT_SPKTERR      CSI_IER0_SPKTERRIE     /*!< Shorter packet than expected error interrupt       */
#define DCMIPP_CSI_IT_IDERR        CSI_IER0_IDERRIE       /*!< Data ID information error interrupt                */
#define DCMIPP_CSI_IT_CECCERR      CSI_IER0_CECCERRIE     /*!< Corrected ECC error interrupt                      */
#define DCMIPP_CSI_IT_ECCERR       CSI_IER0_ECCERRIE      /*!< ECC Error interrupt                                */
#define DCMIPP_CSI_IT_CRCERR       CSI_IER0_CRCERRIE      /*!< CRC Error interrupt                                */
#define DCMIPP_CSI_IT_CCFIFO       CSI_IER0_CCFIFOFIE     /*!< Clock changer FIFO full event interrupt            */
#define DCMIPP_CSI_IT_SPKT         CSI_IER0_SPKTIE        /*!< Short packet detection interrupt                   */
#define DCMIPP_CSI_IT_EOF3         CSI_IER0_EOF3IE        /*!< EOF on virtual channel 3 interrupt                 */
#define DCMIPP_CSI_IT_EOF2         CSI_IER0_EOF2IE        /*!< EOF on virtual channel 2 interrupt                 */
#define DCMIPP_CSI_IT_EOF1         CSI_IER0_EOF1IE        /*!< EOF on virtual channel 1 interrupt                 */
#define DCMIPP_CSI_IT_EOF0         CSI_IER0_EOF0IE        /*!< EOF on virtual channel 0 interrupt                 */
#define DCMIPP_CSI_IT_SOF3         CSI_IER0_SOF3IE        /*!< SOF on virtual channel 3 interrupt                 */
#define DCMIPP_CSI_IT_SOF2         CSI_IER0_SOF2IE        /*!< SOF on virtual channel 2 interrupt                 */
#define DCMIPP_CSI_IT_SOF1         CSI_IER0_SOF1IE        /*!< SOF on virtual channel 1 interrupt                 */
#define DCMIPP_CSI_IT_SOF0         CSI_IER0_SOF0IE        /*!< SOF on virtual channel 0 interrupt                 */
#define DCMIPP_CSI_IT_TIM3         CSI_IER0_TIM3IE        /*!< Timer 3 counter value reached interrupt            */
#define DCMIPP_CSI_IT_TIM2         CSI_IER0_TIM2IE        /*!< Timer 2 counter value reached interrupt            */
#define DCMIPP_CSI_IT_TIM1         CSI_IER0_TIM1IE        /*!< Timer 1 counter value reached interrupt            */
#define DCMIPP_CSI_IT_TIM0         CSI_IER0_TIM0IE        /*!< Timer 0 counter value reached interrupt            */
#define DCMIPP_CSI_IT_LB3          CSI_IER0_LB3IE         /*!< Line/byte event 3 reached interrupt                */
#define DCMIPP_CSI_IT_LB2          CSI_IER0_LB2IE         /*!< Line/byte event 2 reached interrupt                */
#define DCMIPP_CSI_IT_LB1          CSI_IER0_LB1IE         /*!< Line/byte event 1 reached interrupt                */
#define DCMIPP_CSI_IT_LB0          CSI_IER0_LB0IE         /*!< Line/byte event 0 reached interrupt                */
#define DCMIPP_CSI_IT_ECTRLDL1     CSI_IER1_ECTRLDL1IE    /*!< Error control on data line 1 interrupt             */
#define DCMIPP_CSI_IT_ESYNCESCDL1  CSI_IER1_ESYNCESCDL1IE /*!< Low-power data transmission synchronization error on
                                                               data line 1 interrupt                              */
#define DCMIPP_CSI_IT_EESCDL1      CSI_IER1_EESCDL1IE     /*!< Escape entry error on data line 1 interrupt        */
#define DCMIPP_CSI_IT_ESOTSYNCDL1  CSI_IER1_ESOTSYNCDL1IE /*!< SOT synchronization error on data line 1 interrupt */
#define DCMIPP_CSI_IT_ESOTDL1      CSI_IER1_ESOTDL1IE     /*!< SOT error on data line 1 interrupt                 */
#define DCMIPP_CSI_IT_ECTRLDL0     CSI_IER1_ECTRLDL0IE    /*!< Error control on data line 0 interrupt             */
#define DCMIPP_CSI_IT_ESYNCESCDL0  CSI_IER1_ESYNCESCDL0IE /*!< Low-power data transmission synchronization error on
                                                               data line 0 interrupt                               */
#define DCMIPP_CSI_IT_EESCDL0      CSI_IER1_EESCDL0IE     /*!< Escape entry error on data line 0 interrupt        */
#define DCMIPP_CSI_IT_ESOTSYNCDL0  CSI_IER1_ESOTSYNCDL0IE /*!< SOT synchronization error on data line 0 interrupt */
#define DCMIPP_CSI_IT_ESOTDL0      CSI_IER1_ESOTDL0IE     /*!< SOT error on data line 0 interrupt                 */
/**
  * @}
  */
/** @defgroup DCMIPP_CSI_Interrupt_Flags  DCMIPP CSI Interrupt Flags
  * @{
  */
#define DCMIPP_CSI_FLAG_SYNCERR      CSI_SR0_SYNCERRF      /*!< Synchronization error flag                        */
#define DCMIPP_CSI_FLAG_WDERR        CSI_SR0_WDERRF        /*!< Watchdog error flag                               */
#define DCMIPP_CSI_FLAG_SPKTERR      CSI_SR0_SPKTERRF      /*!< Shorter packet than expected error flag           */
#define DCMIPP_CSI_FLAG_IDERR        CSI_SR0_IDERRF        /*!< Data ID information error flag                    */
#define DCMIPP_CSI_FLAG_CECCERR      CSI_SR0_CECCERRF      /*!< Corrected ECC error flag                          */
#define DCMIPP_CSI_FLAG_ECCERR       CSI_SR0_ECCERRF       /*!< ECC Error flag                                    */
#define DCMIPP_CSI_FLAG_CRCERR       CSI_SR0_CRCERRF       /*!< CRC Error flag                                    */
#define DCMIPP_CSI_FLAG_CCFIFO       CSI_SR0_CCFIFOFF      /*!< Clock changer FIFO full event flag                */
#define DCMIPP_CSI_FLAG_SPKT         CSI_SR0_SPKTF         /*!< Short packet detection flag                       */
#define DCMIPP_CSI_FLAG_EOF3         CSI_SR0_EOF3F         /*!< EOF on virtual channel 3 flag                     */
#define DCMIPP_CSI_FLAG_EOF2         CSI_SR0_EOF2F         /*!< EOF on virtual channel 2 flag                     */
#define DCMIPP_CSI_FLAG_EOF1         CSI_SR0_EOF1F         /*!< EOF on virtual channel 1 flag                     */
#define DCMIPP_CSI_FLAG_EOF0         CSI_SR0_EOF0F         /*!< EOF on virtual channel 0 flag                     */
#define DCMIPP_CSI_FLAG_SOF3         CSI_SR0_SOF3F         /*!< SOF on virtual channel 3 flag                     */
#define DCMIPP_CSI_FLAG_SOF2         CSI_SR0_SOF2F         /*!< SOF on virtual channel 2 flag                     */
#define DCMIPP_CSI_FLAG_SOF1         CSI_SR0_SOF1F         /*!< SOF on virtual channel 1 flag                     */
#define DCMIPP_CSI_FLAG_SOF0         CSI_SR0_SOF0F         /*!< SOF on virtual channel 0 flag                     */
#define DCMIPP_CSI_FLAG_TIM3         CSI_SR0_TIM3F         /*!< Timer 3 counter value reached flag                */
#define DCMIPP_CSI_FLAG_TIM2         CSI_SR0_TIM2F         /*!< Timer 2 counter value reached flag                */
#define DCMIPP_CSI_FLAG_TIM1         CSI_SR0_TIM1F         /*!< Timer 1 counter value reached flag                */
#define DCMIPP_CSI_FLAG_TIM0         CSI_SR0_TIM0F         /*!< Timer 0 counter value reached flag                */
#define DCMIPP_CSI_FLAG_LB3          CSI_SR0_LB3F          /*!< Line/byte event 3 reached  flag                   */
#define DCMIPP_CSI_FLAG_LB2          CSI_SR0_LB2F          /*!< Line/byte event 2 reached  flag                   */
#define DCMIPP_CSI_FLAG_LB1          CSI_SR0_LB1F          /*!< Line/byte event 1 reached  flag                   */
#define DCMIPP_CSI_FLAG_LB0          CSI_SR0_LB0F          /*!< Line/byte event 0 reached  flag                   */
#define DCMIPP_CSI_FLAG_ECTRLDL1     CSI_SR1_ECTRLDL1F     /*!< Error control on data line 1 flag             */
#define DCMIPP_CSI_FLAG_ESYNCESCDL1  CSI_SR1_ESYNCESCDL1F  /*!< Low-power data transmission synchronization
                                                                error on data line 1 flag                     */
#define DCMIPP_CSI_FLAG_EESCDL1      CSI_SR1_EESCDL1F      /*!< Escape entry error on data line 1 flag        */
#define DCMIPP_CSI_FLAG_ESOTSYNCDL1  CSI_SR1_ESOTSYNCDL1F  /*!< SOT synchronization error on data line 1 flag */
#define DCMIPP_CSI_FLAG_ESOTDL1      CSI_SR1_ESOTDL1F      /*!< SOT error on data line 1 flag                 */
#define DCMIPP_CSI_FLAG_ECTRLDL0     CSI_SR1_ECTRLDL0F     /*!< Error control on data line 0 flag             */
#define DCMIPP_CSI_FLAG_ESYNCESCDL0  CSI_SR1_ESYNCESCDL0F  /*!< Low-power data transmission synchronization
                                                                error on data line 0 flag                     */
#define DCMIPP_CSI_FLAG_EESCDL0      CSI_SR1_EESCDL0F      /*!< Escape entry error on data line 0 flag        */
#define DCMIPP_CSI_FLAG_ESOTSYNCDL0  CSI_SR1_ESOTSYNCDL0F  /*!< SOT synchronization error on data line 0 flag */
#define DCMIPP_CSI_FLAG_ESOTDL0      CSI_SR1_ESOTDL0F      /*!< SOT error on data line 0 flag                 */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup DCMIPP_Exported_Macros DCMIPP Exported Macros
  * @brief    DCMIPP Exported Macros
  * @{
  */
/**
  * @brief  Enable the specified DCMIPP interrupts.
  * @param  __HANDLE__    DCMIPP handle
  * @param  __INTERRUPT__ specifies the DCMIPP interrupt sources to be enabled.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_IT_AXI_TRANSFER_ERR IPPLUG AXI Transfer error interrupt
  *            @arg DCMIPP_IT_PARALLEL_SYNC_ERR Synchronization error interrupt on parallel interface
  *            @arg DCMIPP_IT_PIPE0_FRAME Frame capture complete interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE0_VSYNC Vertical sync interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE0_LINE Multi-line capture complete interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE0_LIMIT Limit interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE0_OVR Overrun interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE1_LINE Multi-line capture complete interrupt for the pipe1
  *            @arg DCMIPP_IT_PIPE1_FRAME Frame capture complete interrupt for the pipe1
  *            @arg DCMIPP_IT_PIPE1_VSYNC Vertical sync interrupt for the pipe1
  *            @arg DCMIPP_IT_PIPE1_OVR Overrun interrupt for the pipe1
  *            @arg DCMIPP_IT_PIPE2_LINE Multi-line capture complete interrupt for the pipe2
  *            @arg DCMIPP_IT_PIPE2_FRAME Frame capture complete interrupt for the pipe2
  *            @arg DCMIPP_IT_PIPE2_VSYNC Vertical sync interrupt for the pipe2
  *            @arg DCMIPP_IT_PIPE2_OVR Overrun interrupt for the pipe2
  * @retval None
  */
#define __HAL_DCMIPP_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->CMIER |= (__INTERRUPT__))

/**
  * @brief  Disable the specified DCMIPP interrupts.
  * @param  __HANDLE__    DCMIPP handle
  * @param  __INTERRUPT__ specifies the DCMIPP interrupt sources to be disabled.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_IT_AXI_TRANSFER_ERR IPPLUG AXI Transfer error interrupt
  *            @arg DCMIPP_IT_PARALLEL_SYNC_ERR Synchronization error interrupt on parallel interface
  *            @arg DCMIPP_IT_PIPE0_FRAME Frame capture complete interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE0_VSYNC Vertical sync interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE0_LINE Multi-line capture complete interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE0_LIMIT Limit interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE0_OVR Overrun interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE1_LINE Multi-line capture complete interrupt for the pipe1
  *            @arg DCMIPP_IT_PIPE1_FRAME Frame capture complete interrupt for the pipe1
  *            @arg DCMIPP_IT_PIPE1_VSYNC Vertical sync interrupt for the pipe1
  *            @arg DCMIPP_IT_PIPE1_OVR Overrun interrupt for the pipe1
  *            @arg DCMIPP_IT_PIPE2_LINE Multi-line capture complete interrupt for the pipe2
  *            @arg DCMIPP_IT_PIPE2_FRAME Frame capture complete interrupt for the pipe2
  *            @arg DCMIPP_IT_PIPE2_VSYNC Vertical sync interrupt for the pipe2
  *            @arg DCMIPP_IT_PIPE2_OVR Overrun interrupt for the pipe2
  * @retval None
  */
#define __HAL_DCMIPP_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->CMIER &= ~(__INTERRUPT__))

/**
  * @brief  Get the DCMIPP pending interrupt flags.
  * @param  __HANDLE__ DCMIPP handle
  * @param  __FLAG__   Get the specified flag.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_FLAG_AXI_TRANSFER_ERR IPPLUG AXI Transfer error interrupt flag
  *            @arg DCMIPP_FLAG_PARALLEL_SYNC_ERR Synchronization error interrupt flag on parallel interface
  *            @arg DCMIPP_FLAG_PIPE0_FRAME Frame capture complete interrupt flag for the pipe0
  *            @arg DCMIPP_FLAG_PIPE0_VSYNC Vertical sync interrupt flag for the pipe0
  *            @arg DCMIPP_FLAG_PIPE0_LINE Multi-line capture complete interrupt flag for the pipe0
  *            @arg DCMIPP_FLAG_PIPE0_LIMIT Limit interrupt flag for the pipe0
  *            @arg DCMIPP_FLAG_PIPE0_OVR Overrun interrupt flag for the pipe0
  *            @arg DCMIPP_FLAG_PIPE1_LINE Multi-line capture complete interrupt flag for the pipe1
  *            @arg DCMIPP_FLAG_PIPE1_FRAME Frame capture complete interrupt flag for the pipe1
  *            @arg DCMIPP_FLAG_PIPE1_VSYNC Vertical sync interrupt flag for the pipe1
  *            @arg DCMIPP_FLAG_PIPE1_OVR Overrun interrupt flag for the pipe1
  *            @arg DCMIPP_FLAG_PIPE2_LINE Multi-line capture complete interrupt flag for the pipe2
  *            @arg DCMIPP_FLAG_PIPE2_FRAME Frame capture complete interrupt flag for the pipe2
  *            @arg DCMIPP_FLAG_PIPE2_VSYNC Vertical sync interrupt flag for the pipe2
  *            @arg DCMIPP_FLAG_PIPE2_OVR Overrun interrupt flag for the pipe2
  * @retval The state of FLAG (SET or RESET).
  */
#define __HAL_DCMIPP_GET_FLAG(__HANDLE__, __FLAG__)  ((__HANDLE__)->Instance->CMSR2 & (__FLAG__))

/**
  * @brief  Clear the DCMIPP pending interrupt flags.
  * @param  __HANDLE__ DCMIPP handle
  * @param  __FLAG__   specifies the flag to clear.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_FLAG_AXI_TRANSFER_ERR IPPLUG AXI Transfer error interrupt
  *            @arg DCMIPP_FLAG_PARALLEL_SYNC_ERR Synchronization error interrupt on parallel interface
  *            @arg DCMIPP_FLAG_PIPE0_FRAME Frame capture complete interrupt for the pipe0
  *            @arg DCMIPP_FLAG_PIPE0_VSYNC Vertical sync interrupt for the pipe0
  *            @arg DCMIPP_FLAG_PIPE0_LINE Multi-line capture complete interrupt for the pipe0
  *            @arg DCMIPP_FLAG_PIPE0_LIMIT Limit interrupt for the pipe0
  *            @arg DCMIPP_FLAG_PIPE0_OVR Overrun interrupt for the pipe0
  *            @arg DCMIPP_FLAG_PIPE1_LINE Multi-line capture complete interrupt for the pipe1
  *            @arg DCMIPP_FLAG_PIPE1_FRAME Frame capture complete interrupt for the pipe1
  *            @arg DCMIPP_FLAG_PIPE1_VSYNC Vertical sync interrupt for the pipe1
  *            @arg DCMIPP_FLAG_PIPE1_OVR Overrun interrupt for the pipe1
  *            @arg DCMIPP_FLAG_PIPE2_LINE Multi-line capture complete interrupt for the pipe2
  *            @arg DCMIPP_FLAG_PIPE2_FRAME Frame capture complete interrupt for the pipe2
  *            @arg DCMIPP_FLAG_PIPE2_VSYNC Vertical sync interrupt for the pipe2
  *            @arg DCMIPP_FLAG_PIPE2_OVR Overrun interrupt for the pipe2
  * @retval None
  */
#define __HAL_DCMIPP_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->CMFCR = (__FLAG__))

/**
  * @brief  Checks whether the specified DCMIPP interrupt is enabled or not.
  * @param  __HANDLE__    DCMIPP handle
  * @param  __INTERRUPT__ specifies the DCMIPP interrupt sources to be checked.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_IT_AXI_TRANSFER_ERR IPPLUG AXI Transfer error interrupt
  *            @arg DCMIPP_IT_PARALLEL_SYNC_ERR Synchronization error interrupt on parallel interface
  *            @arg DCMIPP_IT_PIPE0_FRAME Frame capture complete interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE0_VSYNC Vertical sync interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE0_LINE Multi-line capture complete interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE0_LIMIT Limit interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE0_OVR Overrun interrupt for the pipe0
  *            @arg DCMIPP_IT_PIPE1_LINE Multi-line capture complete interrupt for the pipe1
  *            @arg DCMIPP_IT_PIPE1_FRAME Frame capture complete interrupt for the pipe1
  *            @arg DCMIPP_IT_PIPE1_VSYNC Vertical sync interrupt for the pipe1
  *            @arg DCMIPP_IT_PIPE1_OVR Overrun interrupt for the pipe1
  *            @arg DCMIPP_IT_PIPE2_LINE Multi-line capture complete interrupt for the pipe2
  *            @arg DCMIPP_IT_PIPE2_FRAME Frame capture complete interrupt for the pipe2
  *            @arg DCMIPP_IT_PIPE2_VSYNC Vertical sync interrupt for the pipe2
  *            @arg DCMIPP_IT_PIPE2_OVR Overrun interrupt for the pipe2
  * @retval The state of DCMIPP interrupt (SET or RESET).
  */
#define __HAL_DCMIPP_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->CMIER & \
                                                                 (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/**
  * @brief  Enable the specified DCMIPP CSI interrupts.
  * @param  __HANDLE__    CSI handle
  * @param  __INTERRUPT__ specifies the DCMIPP CSI interrupt sources to be enabled.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_CSI_IT_SYNCERR Synchronization error interrupt
  *            @arg DCMIPP_CSI_IT_WDERR   Watchdog error interrupt
  *            @arg DCMIPP_CSI_IT_SPKTERR Shorter packet than expected error interrupt
  *            @arg DCMIPP_CSI_IT_IDERR   Data ID information error interrupt
  *            @arg DCMIPP_CSI_IT_CECCERR Corrected ECC error interrupt
  *            @arg DCMIPP_CSI_IT_ECCERR  ECC Error interrupt
  *            @arg DCMIPP_CSI_IT_CRCERR  CRC Error interrupt
  *            @arg DCMIPP_CSI_IT_CCFIFO  Clock changer FIFO full event interrupt
  *            @arg DCMIPP_CSI_IT_SPKT    Short packet detection interrupt
  *            @arg DCMIPP_CSI_IT_EOF3    EOF on virtual channel 3 interrupt
  *            @arg DCMIPP_CSI_IT_EOF2    EOF on virtual channel 2 interrupt
  *            @arg DCMIPP_CSI_IT_EOF1    EOF on virtual channel 1 interrupt
  *            @arg DCMIPP_CSI_IT_EOF0    EOF on virtual channel 0 interrupt
  *            @arg DCMIPP_CSI_IT_SOF3    SOF on virtual channel 3 interrupt
  *            @arg DCMIPP_CSI_IT_SOF2    SOF on virtual channel 2 interrupt
  *            @arg DCMIPP_CSI_IT_SOF1    SOF on virtual channel 1 interrupt
  *            @arg DCMIPP_CSI_IT_SOF0    SOF on virtual channel 0 interrupt
  *            @arg DCMIPP_CSI_IT_TIM3    Timer 3 counter value reached interrupt
  *            @arg DCMIPP_CSI_IT_TIM2    Timer 2 counter value reached interrupt
  *            @arg DCMIPP_CSI_IT_TIM1    Timer 1 counter value reached interrupt
  *            @arg DCMIPP_CSI_IT_TIM0    Timer 0 counter value reached interrupt
  *            @arg DCMIPP_CSI_IT_LB3     Line/byte event 3 reached interrupt
  *            @arg DCMIPP_CSI_IT_LB2     Line/byte event 2 reached interrupt
  *            @arg DCMIPP_CSI_IT_LB1     Line/byte event 1 reached interrupt
  *            @arg DCMIPP_CSI_IT_LB0     Line/byte event 0 reached interrupt
  * @retval None
  */
#define __HAL_DCMIPP_CSI_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->IER0 |= (__INTERRUPT__))
/**
  * @brief  Disable the specified DCMIPP CSI interrupts.
  * @param  __HANDLE__    CSI handle
  * @param  __INTERRUPT__ specifies the DCMIPP CSI interrupt sources to be enabled.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_CSI_IT_SYNCERR Synchronization error interrupt
  *            @arg DCMIPP_CSI_IT_WDERR   Watchdog error interrupt
  *            @arg DCMIPP_CSI_IT_SPKTERR Shorter packet than expected error interrupt
  *            @arg DCMIPP_CSI_IT_IDERR   Data ID information error interrupt
  *            @arg DCMIPP_CSI_IT_CECCERR Corrected ECC error interrupt
  *            @arg DCMIPP_CSI_IT_ECCERR  ECC Error interrupt
  *            @arg DCMIPP_CSI_IT_CRCERR  CRC Error interrupt
  *            @arg DCMIPP_CSI_IT_CCFIFO  Clock changer FIFO full event interrupt
  *            @arg DCMIPP_CSI_IT_SPKT    Short packet detection interrupt
  *            @arg DCMIPP_CSI_IT_EOF3    EOF on virtual channel 3 interrupt
  *            @arg DCMIPP_CSI_IT_EOF2    EOF on virtual channel 2 interrupt
  *            @arg DCMIPP_CSI_IT_EOF1    EOF on virtual channel 1 interrupt
  *            @arg DCMIPP_CSI_IT_EOF0    EOF on virtual channel 0 interrupt
  *            @arg DCMIPP_CSI_IT_SOF3    SOF on virtual channel 3 interrupt
  *            @arg DCMIPP_CSI_IT_SOF2    SOF on virtual channel 2 interrupt
  *            @arg DCMIPP_CSI_IT_SOF1    SOF on virtual channel 1 interrupt
  *            @arg DCMIPP_CSI_IT_SOF0    SOF on virtual channel 0 interrupt
  *            @arg DCMIPP_CSI_IT_TIM3    Timer 3 counter value reached interrupt
  *            @arg DCMIPP_CSI_IT_TIM2    Timer 2 counter value reached interrupt
  *            @arg DCMIPP_CSI_IT_TIM1    Timer 1 counter value reached interrupt
  *            @arg DCMIPP_CSI_IT_TIM0    Timer 0 counter value reached interrupt
  *            @arg DCMIPP_CSI_IT_LB3     Line/byte event 3 reached interrupt
  *            @arg DCMIPP_CSI_IT_LB2     Line/byte event 2 reached interrupt
  *            @arg DCMIPP_CSI_IT_LB1     Line/byte event 1 reached interrupt
  *            @arg DCMIPP_CSI_IT_LB0     Line/byte event 0 reached interrupt
  * @retval None
  */
#define __HAL_DCMIPP_CSI_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->IER0 &= ~(__INTERRUPT__))
/**
  * @brief  Get the DCMIPP CSI pending interrupt flags.
  * @param  __HANDLE__    CSI handle
  * @param  __FLAG__   Get the specified flag.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_CSI_FLAG_SYNCERR Synchronization error flag
  *            @arg DCMIPP_CSI_FLAG_WDERR   Watchdog error flag
  *            @arg DCMIPP_CSI_FLAG_SPKTERR Shorter packet than expected error flag
  *            @arg DCMIPP_CSI_FLAG_IDERR   Data ID information error flag
  *            @arg DCMIPP_CSI_FLAG_CECCERR Corrected ECC error flag
  *            @arg DCMIPP_CSI_FLAG_ECCERR  ECC Error flag
  *            @arg DCMIPP_CSI_FLAG_CRCERR  CRC Error flag
  *            @arg DCMIPP_CSI_FLAG_CCFIFO  Clock changer FIFO full event flag
  *            @arg DCMIPP_CSI_FLAG_SPKT    Short packet detection flag
  *            @arg DCMIPP_CSI_FLAG_EOF3    EOF on virtual channel 3 flag
  *            @arg DCMIPP_CSI_FLAG_EOF2    EOF on virtual channel 2 flag
  *            @arg DCMIPP_CSI_FLAG_EOF1    EOF on virtual channel 1 flag
  *            @arg DCMIPP_CSI_FLAG_EOF0    EOF on virtual channel 0 flag
  *            @arg DCMIPP_CSI_FLAG_SOF3    SOF on virtual channel 3 flag
  *            @arg DCMIPP_CSI_FLAG_SOF2    SOF on virtual channel 2 flag
  *            @arg DCMIPP_CSI_FLAG_SOF1    SOF on virtual channel 1 flag
  *            @arg DCMIPP_CSI_FLAG_SOF0    SOF on virtual channel 0 flag
  *            @arg DCMIPP_CSI_FLAG_TIM3    Timer 3 counter value reached flag
  *            @arg DCMIPP_CSI_FLAG_TIM2    Timer 2 counter value reached flag
  *            @arg DCMIPP_CSI_FLAG_TIM1    Timer 1 counter value reached flag
  *            @arg DCMIPP_CSI_FLAG_TIM0    Timer 0 counter value reached flag
  *            @arg DCMIPP_CSI_FLAG_LB3     Line/byte event 3 reached flag
  *            @arg DCMIPP_CSI_FLAG_LB2     Line/byte event 2 reached flag
  *            @arg DCMIPP_CSI_FLAG_LB1     Line/byte event 1 reached flag
  *            @arg DCMIPP_CSI_FLAG_LB0     Line/byte event 0 reached flag
  * @retval None
  */
#define __HAL_DCMIPP_CSI_GET_FLAG(__HANDLE__, __FLAG__)((__HANDLE__)->IER0 & (__FLAG__))
/**
  * @brief  Clear the DCMIPP CSI pending interrupt flags.
  * @param  __HANDLE__    CSI handle
  * @param  __FLAG__   specifies the flag to clear.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_CSI_FLAG_SYNCERR Synchronization error flag
  *            @arg DCMIPP_CSI_FLAG_WDERR   Watchdog error flag
  *            @arg DCMIPP_CSI_FLAG_SPKTERR Shorter packet than expected error flag
  *            @arg DCMIPP_CSI_FLAG_IDERR   Data ID information error flag
  *            @arg DCMIPP_CSI_FLAG_CECCERR Corrected ECC error flag
  *            @arg DCMIPP_CSI_FLAG_ECCERR  ECC Error flag
  *            @arg DCMIPP_CSI_FLAG_CRCERR  CRC Error flag
  *            @arg DCMIPP_CSI_FLAG_CCFIFO  Clock changer FIFO full event flag
  *            @arg DCMIPP_CSI_FLAG_SPKT    Short packet detection flag
  *            @arg DCMIPP_CSI_FLAG_EOF3    EOF on virtual channel 3 flag
  *            @arg DCMIPP_CSI_FLAG_EOF2    EOF on virtual channel 2 flag
  *            @arg DCMIPP_CSI_FLAG_EOF1    EOF on virtual channel 1 flag
  *            @arg DCMIPP_CSI_FLAG_EOF0    EOF on virtual channel 0 flag
  *            @arg DCMIPP_CSI_FLAG_SOF3    SOF on virtual channel 3 flag
  *            @arg DCMIPP_CSI_FLAG_SOF2    SOF on virtual channel 2 flag
  *            @arg DCMIPP_CSI_FLAG_SOF1    SOF on virtual channel 1 flag
  *            @arg DCMIPP_CSI_FLAG_SOF0    SOF on virtual channel 0 flag
  *            @arg DCMIPP_CSI_FLAG_TIM3    Timer 3 counter value reached flag
  *            @arg DCMIPP_CSI_FLAG_TIM2    Timer 2 counter value reached flag
  *            @arg DCMIPP_CSI_FLAG_TIM1    Timer 1 counter value reached flag
  *            @arg DCMIPP_CSI_FLAG_TIM0    Timer 0 counter value reached flag
  *            @arg DCMIPP_CSI_FLAG_LB3     Line/byte event 3 reached flag
  *            @arg DCMIPP_CSI_FLAG_LB2     Line/byte event 2 reached flag
  *            @arg DCMIPP_CSI_FLAG_LB1     Line/byte event 1 reached flag
  *            @arg DCMIPP_CSI_FLAG_LB0     Line/byte event 0 reached flag
  * @retval None
  */
#define __HAL_DCMIPP_CSI_CLEAR_FLAG(__HANDLE__, __FLAG__)((__HANDLE__)->FCR0 = (__FLAG__))
/**
  * @brief  Checks whether the specified DCMIPP interrupt is enabled or not.
  * @param  __HANDLE__    DCMIPP handle
  * @param  __INTERRUPT__ specifies the DCMIPP interrupt sources to be checked.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_CSI_IT_SYNCERR Synchronization error interrupt
  *            @arg DCMIPP_CSI_IT_WDERR   Watchdog error interrupt
  *            @arg DCMIPP_CSI_IT_SPKTERR Shorter packet than expected error interrupt
  *            @arg DCMIPP_CSI_IT_IDERR   Data ID information error interrupt
  *            @arg DCMIPP_CSI_IT_CECCERR Corrected ECC error interrupt
  *            @arg DCMIPP_CSI_IT_ECCERR  ECC Error interrupt
  *            @arg DCMIPP_CSI_IT_CRCERR  CRC Error interrupt
  *            @arg DCMIPP_CSI_IT_CCFIFO  Clock changer FIFO full event interrupt
  *            @arg DCMIPP_CSI_IT_SPKT    Short packet detection interrupt
  *            @arg DCMIPP_CSI_IT_EOF3    EOF on virtual channel 3 interrupt
  *            @arg DCMIPP_CSI_IT_EOF2    EOF on virtual channel 2 interrupt
  *            @arg DCMIPP_CSI_IT_EOF1    EOF on virtual channel 1 interrupt
  *            @arg DCMIPP_CSI_IT_EOF0    EOF on virtual channel 0 interrupt
  *            @arg DCMIPP_CSI_IT_SOF3    SOF on virtual channel 3 interrupt
  *            @arg DCMIPP_CSI_IT_SOF2    SOF on virtual channel 2 interrupt
  *            @arg DCMIPP_CSI_IT_SOF1    SOF on virtual channel 1 interrupt
  *            @arg DCMIPP_CSI_IT_SOF0    SOF on virtual channel 0 interrupt
  *            @arg DCMIPP_CSI_IT_TIM3    Timer 3 counter value reached interrupt
  *            @arg DCMIPP_CSI_IT_TIM2    Timer 2 counter value reached interrupt
  *            @arg DCMIPP_CSI_IT_TIM1    Timer 1 counter value reached interrupt
  *            @arg DCMIPP_CSI_IT_TIM0    Timer 0 counter value reached interrupt
  *            @arg DCMIPP_CSI_IT_LB3     Line/byte event 3 reached interrupt
  *            @arg DCMIPP_CSI_IT_LB2     Line/byte event 2 reached interrupt
  *            @arg DCMIPP_CSI_IT_LB1     Line/byte event 1 reached interrupt
  *            @arg DCMIPP_CSI_IT_LB0     Line/byte event 0 reached interrupt
  * @retval The state of DCMIPP interrupt (SET or RESET).
  */
#define __HAL_DCMIPP_CSI_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->IER0 & (__INTERRUPT__)) == \
                                                                    (__INTERRUPT__)) ? SET : RESET)
/**
  * @brief  Enable the specified DCMIPP CSI DPHY interrupts.
  * @param  __HANDLE__    CSI handle
  * @param  __INTERRUPT__ specifies the DCMIPP CSI interrupt sources to be enabled.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_CSI_IT_ECTRLDL1    Error control on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_ESYNCESCDL1 Low-power data transmission synchronization error on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_EESCDL1     Escape entry error on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_ESOTSYNCDL1 SOT synchronization error on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_ESOTDL1     SOT error on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_ECTRLDL0    Error control on data line 0 interrupt
  *            @arg DCMIPP_CSI_IT_ESYNCESCDL0 Low-power data transmission synchronization error on data line 0 interrupt
  *            @arg DCMIPP_CSI_IT_EESCDL0     Escape entry error on data line 0 interrupt
  *            @arg DCMIPP_CSI_IT_ESOTSYNCDL0 SOT synchronization error on data line 0 interrupt
  *            @arg DCMIPP_CSI_IT_ESOTDL0     SOT error on data line 0 interrupt
  * @retval None
  */
#define __HAL_DCMIPP_CSI_DPHY_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->IER1 |= (__INTERRUPT__))
/**
  * @brief  Disable the specified DCMIPP CSI DPHY interrupts.
  * @param  __HANDLE__    CSI handle
  * @param  __INTERRUPT__ specifies the DCMIPP CSI interrupt sources to be enabled.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_CSI_IT_ECTRLDL1    Error control on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_ESYNCESCDL1 Low-power data transmission synchronization error on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_EESCDL1     Escape entry error on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_ESOTSYNCDL1 SOT synchronization error on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_ESOTDL1     SOT error on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_ECTRLDL0    Error control on data line 0 interrupt
  *            @arg DCMIPP_CSI_IT_ESYNCESCDL0 Low-power data transmission synchronization error on data line 0 interrupt
  *            @arg DCMIPP_CSI_IT_EESCDL0     Escape entry error on data line 0 interrupt
  *            @arg DCMIPP_CSI_IT_ESOTSYNCDL0 SOT synchronization error on data line 0 interrupt
  *            @arg DCMIPP_CSI_IT_ESOTDL0     SOT error on data line 0 interrupt
  * @retval None
  */
#define __HAL_DCMIPP_CSI_DPHY_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->IER1 &= ~(__INTERRUPT__))
/**
  * @brief  Get the DCMIPP CSI DPHY pending interrupt flags.
  * @param  __HANDLE__    CSI handle
  * @param  __FLAG__   Get the specified flag.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_CSI_FLAG_ECTRLDL1    Error control on data line 1 flag
  *            @arg DCMIPP_CSI_FLAG_ESYNCESCDL1 Low-power data transmission synchronization error on data line 1 flag
  *            @arg DCMIPP_CSI_FLAG_EESCDL1     Escape entry error on data line 1 flag
  *            @arg DCMIPP_CSI_FLAG_ESOTSYNCDL1 SOT synchronization error on data line 1 flag
  *            @arg DCMIPP_CSI_FLAG_ESOTDL1     SOT error on data line 1 flag
  *            @arg DCMIPP_CSI_FLAG_ECTRLDL0    Error control on data line 0 flag
  *            @arg DCMIPP_CSI_FLAG_ESYNCESCDL0 Low-power data transmission synchronization error on data line 0 flag
  *            @arg DCMIPP_CSI_FLAG_EESCDL0     Escape entry error on data line 0 flag
  *            @arg DCMIPP_CSI_FLAG_ESOTSYNCDL0 SOT synchronization error on data line 0 flag
  *            @arg DCMIPP_CSI_FLAG_ESOTDL0     SOT error on data line 0 flag
  * @retval None
  */
#define __HAL_DCMIPP_CSI_GET_DPHY_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->IER1 & (__FLAG__))
/**
  * @brief  Clear the DCMIPP CSI DPHY pending interrupt flags.
  * @param  __HANDLE__    CSI handle
  * @param  __FLAG__   specifies the flag to clear.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_CSI_FLAG_ECTRLDL1    Error control on data line 1 flag
  *            @arg DCMIPP_CSI_FLAG_ESYNCESCDL1 Low-power data transmission synchronization error on data line 1 flag
  *            @arg DCMIPP_CSI_FLAG_EESCDL1     Escape entry error on data line 1 flag
  *            @arg DCMIPP_CSI_FLAG_ESOTSYNCDL1 SOT synchronization error on data line 1 flag
  *            @arg DCMIPP_CSI_FLAG_ESOTDL1     SOT error on data line 1 flag
  *            @arg DCMIPP_CSI_FLAG_ECTRLDL0    Error control on data line 0 flag
  *            @arg DCMIPP_CSI_FLAG_ESYNCESCDL0 Low-power data transmission synchronization error on data line 0 flag
  *            @arg DCMIPP_CSI_FLAG_EESCDL0     Escape entry error on data line 0 flag
  *            @arg DCMIPP_CSI_FLAG_ESOTSYNCDL0 SOT synchronization error on data line 0 flag
  *            @arg DCMIPP_CSI_FLAG_ESOTDL0     SOT error on data line 0 flag
  * @retval None
  */
#define __HAL_DCMIPP_CSI_CLEAR_DPHY_FLAG(__HANDLE__, __FLAG__)((__HANDLE__)->FCR1 = (__FLAG__))
/**
  * @brief  Checks whether the specified DCMIPP CSI DPHY interrupt is enabled or not.
  * @param  __HANDLE__    CSI handle
  * @param  __INTERRUPT__ specifies the DCMIPP CSI D-PHY interrupt sources to be checked.
  *         This parameter can be any combination of the following values:
  *            @arg DCMIPP_CSI_IT_ECTRLDL1    Error control on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_ESYNCESCDL1 Low-power data transmission synchronization error on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_EESCDL1     Escape entry error on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_ESOTSYNCDL1 SOT synchronization error on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_ESOTDL1     SOT error on data line 1 interrupt
  *            @arg DCMIPP_CSI_IT_ECTRLDL0    Error control on data line 0 interrupt
  *            @arg DCMIPP_CSI_IT_ESYNCESCDL0 Low-power data transmission synchronization error on data line 0 interrupt
  *            @arg DCMIPP_CSI_IT_EESCDL0     Escape entry error on data line 0 interrupt
  *            @arg DCMIPP_CSI_IT_ESOTSYNCDL0 SOT synchronization error on data line 0 interrupt
  *            @arg DCMIPP_CSI_IT_ESOTDL0     SOT error on data line 0 interrupt
  * @retval None
  */
#define __HAL_DCMIPP_CSI_GET_DPHY_IT_SOURCE(__HANDLE__,  __INTERRUPT__)((((__HANDLE__)->IER1 & (__INTERRUPT__)) == \
                                                                         (__INTERRUPT__)) ? SET : RESET)
/**
  * @}
  */
/* Exported functions --------------------------------------------------------*/
/** @addtogroup DCMIPP_Exported_Functions
  * @{
  */

/** @addtogroup DCMIPP_Initialization_De-Initialization_Functions DCMIPP Initialization De-Initialization Functions
  * @brief      Initialization and De-Initialization Functions
  * @{
  */
HAL_StatusTypeDef HAL_DCMIPP_Init(DCMIPP_HandleTypeDef *hdcmipp);
HAL_StatusTypeDef HAL_DCMIPP_DeInit(DCMIPP_HandleTypeDef *hdcmipp);
void HAL_DCMIPP_MspInit(DCMIPP_HandleTypeDef *hdcmipp);
void HAL_DCMIPP_MspDeInit(DCMIPP_HandleTypeDef *hdcmipp);
/**
  * @}
  */

/** @defgroup DCMIPP_Configuration_Functions DCMIPP Configuration Functions
  * @brief    Configuration Functions
  * @{
  */

HAL_StatusTypeDef HAL_DCMIPP_PARALLEL_SetConfig(DCMIPP_HandleTypeDef *hdcmipp,
                                                const DCMIPP_ParallelConfTypeDef *pParallelConfig);
HAL_StatusTypeDef HAL_DCMIPP_CSI_PIPE_SetConfig(DCMIPP_HandleTypeDef *hdcmipp,
                                                uint32_t Pipe, const DCMIPP_CSI_PIPE_ConfTypeDef *pCSI_PipeConfig);
HAL_StatusTypeDef HAL_DCMIPP_CSI_SetConfig(const DCMIPP_HandleTypeDef *hdcmipp,
                                           const DCMIPP_CSI_ConfTypeDef *pCSI_Config);
HAL_StatusTypeDef HAL_DCMIPP_CSI_SetVCFilteringConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t VirtualChannel,
                                                      const DCMIPP_CSI_VCFilteringConfTypeDef *pVCFilteringConfig);
HAL_StatusTypeDef HAL_DCMIPP_CSI_SetVCConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t VirtualChannel,
                                             uint32_t DataTypeFormat);


HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                            const DCMIPP_PipeConfTypeDef *pPipeConfig);
HAL_StatusTypeDef HAL_DCMIPP_SetIPPlugConfig(DCMIPP_HandleTypeDef *hdcmipp,
                                             const DCMIPP_IPPlugConfTypeDef *pIPPlugConfig);
/**
  * @}
  */

/** @addtogroup DCMIPP_IO_operation_Functions DCMIPP IO operation Functions
  * @brief      IO Operation Functions
  * @{
  */
HAL_StatusTypeDef HAL_DCMIPP_PIPE_Start(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe, uint32_t DstAddress,
                                        uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DoubleBufferStart(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe, uint32_t DstAddress0,
                                                    uint32_t DstAddress1, uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_CSI_PIPE_Start(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe, uint32_t VirtualChannel,
                                            uint32_t DstAddress, uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_CSI_PIPE_DoubleBufferStart(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                        uint32_t VirtualChannel, uint32_t DstAddress0,
                                                        uint32_t DstAddress1, uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SemiPlanarStart(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                  DCMIPP_SemiPlanarDstAddressTypeDef *pSemiPlanarDstAddress,
                                                  uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SemiPlanarDoubleBufferStart(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                              DCMIPP_SemiPlanarDstAddressTypeDef
                                                              *pSemiPlanarDstAddress0,
                                                              DCMIPP_SemiPlanarDstAddressTypeDef
                                                              *pSemiPlanarDstAddress1, uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_FullPlanarStart(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                  DCMIPP_FullPlanarDstAddressTypeDef *pFullPlanarDstAddress,
                                                  uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_FullPlanarDoubleBufferStart(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                              DCMIPP_FullPlanarDstAddressTypeDef
                                                              *pFullPlanarDstAddress0,
                                                              DCMIPP_FullPlanarDstAddressTypeDef
                                                              *pFullPlanarDstAddress1, uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_CSI_PIPE_SemiPlanarStart(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                      uint32_t VirtualChannel,
                                                      DCMIPP_SemiPlanarDstAddressTypeDef *pSemiPlanarDstAddress,
                                                      uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_CSI_PIPE_SemiPlanarDoubleBufferStart(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                                  uint32_t VirtualChannel,
                                                                  DCMIPP_SemiPlanarDstAddressTypeDef
                                                                  *pSemiPlanarDstAddress0,
                                                                  DCMIPP_SemiPlanarDstAddressTypeDef
                                                                  *pSemiPlanarDstAddress1, uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_CSI_PIPE_FullPlanarStart(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                      uint32_t VirtualChannel,
                                                      DCMIPP_FullPlanarDstAddressTypeDef *pFullPlanarDstAddress,
                                                      uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_CSI_PIPE_FullPlanarDoubleBufferStart(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                                  uint32_t VirtualChannel,
                                                                  DCMIPP_FullPlanarDstAddressTypeDef
                                                                  *pFullPlanarDstAddress0,
                                                                  DCMIPP_FullPlanarDstAddressTypeDef
                                                                  *pFullPlanarDstAddress1,
                                                                  uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_Stop(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_CSI_PIPE_Stop(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe, uint32_t VirtualChannel);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_Suspend(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_Resume(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

/**
  * @}
  */

/** @addtogroup DCMIPP_IRQ_and_Callbacks_Functions DCMIPP IRQ and Callbacks Functions
  * @brief      IRQ and Callbacks functions
  * @{
  */
/** @addtogroup DCMIPP_IRQHandler_Function IRQHandler Function
  * @{
  */
void HAL_DCMIPP_IRQHandler(DCMIPP_HandleTypeDef *hdcmipp);
void HAL_DCMIPP_CSI_IRQHandler(DCMIPP_HandleTypeDef *hdcmipp);
/**
  * @}
  */
/** @addtogroup DCMIPP_Callback_Functions Callback Functions
  * @{
  */
void HAL_DCMIPP_PIPE_FrameEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
void HAL_DCMIPP_PIPE_VsyncEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
void HAL_DCMIPP_PIPE_LineEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
void HAL_DCMIPP_PIPE_LimitEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
void HAL_DCMIPP_PIPE_ErrorCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
void HAL_DCMIPP_ErrorCallback(DCMIPP_HandleTypeDef *hdcmipp);
/**
  * @}
  */

/** @addtogroup DCMIPP_CSI_Callback_Functions CSI Callback Functions
  * @{
  */
void HAL_DCMIPP_CSI_StartOfFrameEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t VirtualChannel);
void HAL_DCMIPP_CSI_EndOfFrameEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t VirtualChannel);
void HAL_DCMIPP_CSI_TimerCounterEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Timer);
void HAL_DCMIPP_CSI_LineByteEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Counter);
void HAL_DCMIPP_CSI_LineErrorCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t DataLane);
void HAL_DCMIPP_CSI_ClockChangerFifoFullEventCallback(DCMIPP_HandleTypeDef *hdcmipp);
void HAL_DCMIPP_CSI_ShortPacketDetectionEventCallback(DCMIPP_HandleTypeDef *hdcmipp);
/**
  * @}
  */
/** @addtogroup DCMIPP_RegisterCallback_Functions Register Callback Functions
  * @{
  */
/* Callbacks Register/UnRegister functions  ***********************************/
#if (USE_HAL_DCMIPP_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_DCMIPP_RegisterCallback(DCMIPP_HandleTypeDef *hdcmipp, HAL_DCMIPP_CallbackIDTypeDef CallbackID,
                                              pDCMIPP_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_DCMIPP_UnRegisterCallback(DCMIPP_HandleTypeDef *hdcmipp,
                                                HAL_DCMIPP_CallbackIDTypeDef CallbackID);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_RegisterCallback(DCMIPP_HandleTypeDef *hdcmipp,
                                                   HAL_DCMIPP_PIPE_CallbackIDTypeDef CallbackID,
                                                   pDCMIPP_PIPE_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_UnRegisterCallback(DCMIPP_HandleTypeDef *hdcmipp,
                                                     HAL_DCMIPP_PIPE_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_DCMIPP_REGISTER_CALLBACKS */
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup DCMIPP_Decimation_Functions DCMIPP Decimation Functions
  * @{
  */
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetBytesDecimationConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                           uint32_t SelectStart, uint32_t SelectMode);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetLinesDecimationConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                           uint32_t SelectStart, uint32_t SelectMode);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetISPDecimationConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                         const DCMIPP_DecimationConfTypeDef *pDecConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableISPDecimation(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableISPDecimation(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetDecimationConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                      const DCMIPP_DecimationConfTypeDef *pDecConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableDecimation(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableDecimation(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
/**
  * @}
  */
/** @defgroup DCMIPP_Crop_Functions DCMIPP Crop Functions
  * @{
  */
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetCropConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                const DCMIPP_CropConfTypeDef *pCropConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableCrop(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableCrop(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
/**
  * @}
  */
/** @defgroup DCMIPP_Line_Event_Functions DCMIPP Line Event Functions
  * @{
  */
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableLineEvent(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe, uint32_t Line);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableLineEvent(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
/**
  * @}
  */
/** @defgroup DCMIPP_LimitEvent_Functions DCMIPP Limit Event Functions
  * @{
  */
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableLimitEvent(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe, uint32_t Limit);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableLimitEvent(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
/**
  * @}
  */
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetISPStatisticExtractionConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                                  uint8_t ModuleID, const
                                                                  DCMIPP_StatisticExtractionConfTypeDef
                                                                  *pStatisticExtractionConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetISPAreaStatisticExtractionConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                                      const DCMIPP_StatisticExtractionAreaConfTypeDef
                                                                      *pStatisticExtractionAreaConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableISPStatisticExtraction(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                               uint8_t ModuleID);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableISPStatisticExtraction(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                                uint8_t ModuleID);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableISPAreaStatisticExtraction(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableISPAreaStatisticExtraction(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetDownsizeConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                    const DCMIPP_DownsizeTypeDef *pDownsizeConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableDownsize(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableDownsize(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetISPBlackLevelCalibrationConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                                    const DCMIPP_BlackLevelConfTypeDef
                                                                    *pBlackLevelConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableISPBlackLevelCalibration(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableISPBlackLevelCalibration(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableGammaConversion(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableGammaConversion(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
uint32_t HAL_DCMIPP_PIPE_IsEnabledGammaConversion(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetISPBadPixelRemovalConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                              uint32_t Strength);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableISPBadPixelRemoval(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableISPBadPixelRemoval(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetISPCtrlContrastConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                           const DCMIPP_ContrastConfTypeDef *pContrastConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableISPCtrlContrast(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableISPCtrlContrast(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetRegionOfInterestConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                            const DCMIPP_RegionOfInterestConfTypeDef *pROIConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableRegionOfInterest(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                         uint32_t Region);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableRegionOfInterest(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                          uint32_t Region);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetLineWrappingConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                        uint32_t AddressWrap);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableLineWrapping(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableLineWrapping(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetISPExposureConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                       const DCMIPP_ExposureConfTypeDef *pExposureConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableISPExposure(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableISPExposure(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetISPRemovalStatisticConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                               uint32_t NbFirstLines, uint32_t NbLastLines);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableISPRemovalStatistic(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableISPRemovalStatistic(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetISPRawBayer2RGBConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                           const DCMIPP_RawBayer2RGBConfTypeDef *pRawBayer2RGBConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableISPRawBayer2RGB(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableISPRawBayer2RGB(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetYUVConversionConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                         const DCMIPP_ColorConversionConfTypeDef
                                                         *pColorConversionConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableYUVConversion(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableYUVConversion(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetISPColorConversionConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                              const DCMIPP_ColorConversionConfTypeDef
                                                              *pColorConversionConfig);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableISPColorConversion(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableISPColorConversion(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);

void HAL_DCMIPP_PIPE_GetISPDecimationConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                            DCMIPP_DecimationConfTypeDef *pDecConfig);
void HAL_DCMIPP_PIPE_GetISPBlackLevelCalibrationConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                       DCMIPP_BlackLevelConfTypeDef *pBlackLevelConfig);
uint32_t HAL_DCMIPP_PIPE_GetISPBadPixelRemovalConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
void HAL_DCMIPP_PIPE_GetISPStatisticExtractionConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                     uint8_t ModuleID,
                                                     DCMIPP_StatisticExtractionConfTypeDef
                                                     *pStatisticExtractionConfig);
void HAL_DCMIPP_PIPE_GetISPAreaStatisticExtractionConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                         DCMIPP_StatisticExtractionAreaConfTypeDef
                                                         *pStatisticExtractionAreaConfig);
void HAL_DCMIPP_PIPE_GetISPCtrlContrastConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                              DCMIPP_ContrastConfTypeDef *pContrastConfig);
void HAL_DCMIPP_PIPE_GetISPExposureConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                          DCMIPP_ExposureConfTypeDef *pExposureConfig);
void HAL_DCMIPP_PIPE_GetISPRawBayer2RGBConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                              DCMIPP_RawBayer2RGBConfTypeDef *pRawBayer2RGBConfig);
void HAL_DCMIPP_PIPE_GetISPColorConversionConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                 DCMIPP_ColorConversionConfTypeDef *pColorConversionConfig);
void HAL_DCMIPP_PIPE_GetISPRemovalStatisticConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                  uint32_t *NbFirstLines, uint32_t *NbLastLines);

uint32_t HAL_DCMIPP_PIPE_IsEnabledISPRemovalStatistic(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
uint32_t HAL_DCMIPP_PIPE_IsEnabledISPBadPixelRemoval(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
uint32_t HAL_DCMIPP_PIPE_IsEnabledISPDecimation(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
uint32_t HAL_DCMIPP_PIPE_IsEnabledISPBlackLevelCalibration(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
uint32_t HAL_DCMIPP_PIPE_IsEnabledISPExposure(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
uint32_t HAL_DCMIPP_PIPE_IsEnabledISPRawBayer2RGB(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
uint32_t HAL_DCMIPP_PIPE_IsEnabledISPColorConversion(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
uint32_t HAL_DCMIPP_PIPE_IsEnabledISPCtrlContrast(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
uint32_t HAL_DCMIPP_PIPE_IsEnabledISPStatisticExtraction(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                         uint8_t ModuleID);
uint32_t HAL_DCMIPP_PIPE_IsEnabledISPAreaStatisticExtraction(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);


/** @defgroup DCMIPP_PeripheralControl_Functions DCMIPP Peripheral Control Functions
  * @{
  */
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetFrameRate(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe, uint32_t FrameRate);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetCaptureMode(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe, uint32_t CaptureMode);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableCapture(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetMemoryAddress(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe, uint32_t Memory,
                                                   uint32_t DstAddress);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_PARALLEL_SetInputPixelFormat(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                               uint32_t InputPixelFormat);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetPitch(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe, uint32_t LinePitch);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetPixelPackerFormat(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                       uint32_t PixelPackerFormat);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_CSI_EnableShare(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_CSI_DisableShare(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_CSI_ForceDataTypeFormat(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                          uint32_t DataTypeFormat);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_CSI_SetDTMode(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                uint32_t DataTypeMode);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_CSI_SetDTSelection(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe, uint32_t DataTypeID,
                                                     uint32_t DataType);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_CSI_EnableHeader(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_CSI_DisableHeader(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PARALLEL_SetSyncUnmask(DCMIPP_HandleTypeDef *hdcmipp,
                                                    const DCMIPP_EmbeddedSyncUnmaskTypeDef *SyncUnmask);
HAL_StatusTypeDef HAL_DCMIPP_CSI_SetLineByteCounterConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Counter,
                                                          const DCMIPP_CSI_LineByteCounterConfTypeDef *pLineByteConfig);
HAL_StatusTypeDef HAL_DCMIPP_CSI_EnableLineByteCounter(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Counter);
HAL_StatusTypeDef HAL_DCMIPP_CSI_DisableLineByteCounter(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Counter);
HAL_StatusTypeDef HAL_DCMIPP_CSI_SetTimerConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Timer,
                                                const DCMIPP_CSI_TimerConfTypeDef *TimerConfig);
HAL_StatusTypeDef HAL_DCMIPP_CSI_EnableTimer(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Timer);
HAL_StatusTypeDef HAL_DCMIPP_CSI_DisableTimer(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Timer);
HAL_StatusTypeDef HAL_DCMIPP_CSI_SetWatchdogCounterConfig(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Counter);

HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableComponentsSwap(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableComponentsSwap(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableRedBlueSwap(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableRedBlueSwap(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_DisableYUVSwap(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_EnableYUVSwap(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
/**
  * @}
  */

/** @defgroup DCMIPP_Frame_Counter_Functions DCMIPP Frame Counter Functions
  * @{
  */
HAL_StatusTypeDef HAL_DCMIPP_PIPE_SetFrameCounterConfig(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_ResetFrameCounter(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_ReadFrameCounter(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                   uint32_t *pCounter);
/**
  * @}
  */
/** @defgroup DCMIPP_Data_Counter_Functions DCMIPP Data Counter Functions
  * @{
  */
HAL_StatusTypeDef HAL_DCMIPP_PIPE_GetDataCounter(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                 uint32_t *pCounter);
/**
  * @}
  */
HAL_StatusTypeDef HAL_DCMIPP_PIPE_GetISPRemovedBadPixelCounter(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                               uint32_t *pCounter);
HAL_StatusTypeDef HAL_DCMIPP_PIPE_GetISPAccumulatedStatisticsCounter(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe,
                                                                     uint8_t ModuleID, uint32_t *pCounter);
uint32_t HAL_DCMIPP_GetMode(const DCMIPP_HandleTypeDef *hdcmipp);
uint32_t HAL_DCMIPP_PIPE_GetMemoryAddress(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe, uint32_t MemoryAddress);

/** @addtogroup DCMIPP_State_and_Error_Functions DCMIPP State and Error Functions
  * @{
  */
HAL_DCMIPP_StateTypeDef HAL_DCMIPP_GetState(const DCMIPP_HandleTypeDef *hdcmipp);
HAL_DCMIPP_PipeStateTypeDef HAL_DCMIPP_PIPE_GetState(const DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe);
uint32_t HAL_DCMIPP_GetError(const DCMIPP_HandleTypeDef *hdcmipp);

/**
  * @}
  */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup DCMIPP_Private_Macros DCMIPP Private Macros
  * @{
  */
#define IS_DCMIPP_PIPE(PIPE) (((PIPE) == DCMIPP_PIPE0) || \
                              ((PIPE) == DCMIPP_PIPE1) || \
                              ((PIPE) == DCMIPP_PIPE2))
#define IS_DCMIPP_FORMAT(FORMAT) (((FORMAT) == DCMIPP_FORMAT_BYTE)   ||\
                                  ((FORMAT) == DCMIPP_FORMAT_YUV422) ||\
                                  ((FORMAT) == DCMIPP_FORMAT_RGB565) ||\
                                  ((FORMAT) == DCMIPP_FORMAT_RGB666) ||\
                                  ((FORMAT) == DCMIPP_FORMAT_RGB888) ||\
                                  ((FORMAT) == DCMIPP_FORMAT_RAW8  ) ||\
                                  ((FORMAT) == DCMIPP_FORMAT_RAW10 ) ||\
                                  ((FORMAT) == DCMIPP_FORMAT_RAW12 ) ||\
                                  ((FORMAT) == DCMIPP_FORMAT_RAW14 ) ||\
                                  ((FORMAT) == DCMIPP_FORMAT_MONOCHROME_8B)  ||\
                                  ((FORMAT) == DCMIPP_FORMAT_MONOCHROME_10B) ||\
                                  ((FORMAT) == DCMIPP_FORMAT_MONOCHROME_12B) ||\
                                  ((FORMAT) == DCMIPP_FORMAT_MONOCHROME_14B))

#define IS_DCMIPP_PIXEL_PACKER_FORMAT(FORMAT) (((FORMAT) == DCMIPP_PIXEL_PACKER_FORMAT_RGB888_YUV444_1) ||\
                                               ((FORMAT) == DCMIPP_PIXEL_PACKER_FORMAT_RGB565_1)        ||\
                                               ((FORMAT) == DCMIPP_PIXEL_PACKER_FORMAT_ARGB8888)        ||\
                                               ((FORMAT) == DCMIPP_PIXEL_PACKER_FORMAT_RGBA888)         ||\
                                               ((FORMAT) == DCMIPP_PIXEL_PACKER_FORMAT_MONO_Y8_G8_1)    ||\
                                               ((FORMAT) == DCMIPP_PIXEL_PACKER_FORMAT_YUV444_1)        ||\
                                               ((FORMAT) == DCMIPP_PIXEL_PACKER_FORMAT_YUV422_1)        ||\
                                               ((FORMAT) == DCMIPP_PIXEL_PACKER_FORMAT_YUV422_2)        ||\
                                               ((FORMAT) == DCMIPP_PIXEL_PACKER_FORMAT_YUV420_2)        ||\
                                               ((FORMAT) == DCMIPP_PIXEL_PACKER_FORMAT_YUV420_3))

#define IS_DCMIPP_PIXEL_PIPE_PITCH(PITCH) ((((PITCH) & 0xFU) == 0U) && ((PITCH) <= 0x7FFFU))

#define IS_DCMIPP_PCKPOLARITY(POLARITY)(((POLARITY) == DCMIPP_PCKPOLARITY_FALLING) || \
                                        ((POLARITY) == DCMIPP_PCKPOLARITY_RISING))

#define IS_DCMIPP_VSPOLARITY(POLARITY)(((POLARITY) == DCMIPP_VSPOLARITY_LOW) || \
                                       ((POLARITY) == DCMIPP_VSPOLARITY_HIGH))

#define IS_DCMIPP_HSPOLARITY(POLARITY)(((POLARITY) == DCMIPP_HSPOLARITY_LOW) || \
                                       ((POLARITY) == DCMIPP_HSPOLARITY_HIGH))


#define IS_DCMIPP_EXTENDED_DATA_MODE(INTERFACE)(((INTERFACE) == DCMIPP_INTERFACE_8BITS ) ||\
                                                ((INTERFACE) == DCMIPP_INTERFACE_10BITS) ||\
                                                ((INTERFACE) == DCMIPP_INTERFACE_12BITS) ||\
                                                ((INTERFACE) == DCMIPP_INTERFACE_14BITS) ||\
                                                ((INTERFACE) == DCMIPP_INTERFACE_16BITS))

#define IS_DCMIPP_SYNC_MODE(SYNC_MODE) (((SYNC_MODE) == DCMIPP_SYNCHRO_HARDWARE) ||\
                                        ((SYNC_MODE) == DCMIPP_SYNCHRO_EMBEDDED))

#define IS_DCMIPP_SWAP_BITS(SWAP_BITS)  (((SWAP_BITS) == DCMIPP_SWAPBITS_ENABLE) ||\
                                         ((SWAP_BITS) == DCMIPP_SWAPBITS_DISABLE))

#define IS_DCMIPP_SWAP_CYCLES(SWAP_CYCLES) (((SWAP_CYCLES) == DCMIPP_SWAPCYCLES_ENABLE) ||\
                                            ((SWAP_CYCLES) == DCMIPP_SWAPCYCLES_DISABLE))

#define IS_DCMIPP_VCID(VCID) (((VCID) == DCMIPP_VIRTUAL_CHANNEL0) ||\
                              ((VCID) == DCMIPP_VIRTUAL_CHANNEL1) ||\
                              ((VCID) == DCMIPP_VIRTUAL_CHANNEL2) ||\
                              ((VCID) == DCMIPP_VIRTUAL_CHANNEL3))

#define IS_DCMIPP_DATA_TYPE_MODE(DATA_TYPE_MODE) (((DATA_TYPE_MODE) == DCMIPP_DTMODE_DTIDA) ||\
                                                  ((DATA_TYPE_MODE) == DCMIPP_DTMODE_DTIDA_OR_DTIDB) ||\
                                                  ((DATA_TYPE_MODE) == DCMIPP_DTMODE_ALL_EXCEPT_DTIA_DTIB) ||\
                                                  ((DATA_TYPE_MODE) == DCMIPP_DTMODE_ALL))

#define IS_DCMIPP_DATA_TYPE(DATA_TYPE) (((DATA_TYPE) == DCMIPP_DT_RGB565) ||\
                                        ((DATA_TYPE) == DCMIPP_DT_RGB444) ||\
                                        ((DATA_TYPE) == DCMIPP_DT_RGB555) ||\
                                        ((DATA_TYPE) == DCMIPP_DT_RGB666) ||\
                                        ((DATA_TYPE) == DCMIPP_DT_RGB888) ||\
                                        ((DATA_TYPE) == DCMIPP_DT_YUV420_8) ||\
                                        ((DATA_TYPE) == DCMIPP_DT_YUV420_10) ||\
                                        ((DATA_TYPE) == DCMIPP_DT_YUV422_8) ||\
                                        ((DATA_TYPE) == DCMIPP_DT_YUV422_10) ||\
                                        ((DATA_TYPE) == DCMIPP_DT_RAW8) ||\
                                        ((DATA_TYPE) == DCMIPP_DT_RAW10) ||\
                                        ((DATA_TYPE) == DCMIPP_DT_RAW12) ||\
                                        ((DATA_TYPE) == DCMIPP_DT_RAW14))

#define IS_DCMIPP_NUMBER_OF_LANES(DATA_LANE_NB) (((DATA_LANE_NB) == DCMIPP_CSI_ONE_DATA_LANE) ||\
                                                 ((DATA_LANE_NB) == DCMIPP_CSI_TWO_DATA_LANES))

#define IS_DCMIPP_CSI_DATA_LANE_MAPPING(DATA_LANE_MAP) (((DATA_LANE_MAP) == DCMIPP_CSI_PHYSICAL_DATA_LANES) ||\
                                                        ((DATA_LANE_MAP) == DCMIPP_CSI_INVERTED_DATA_LANES))
#define IS_DCMIPP_CSI_DATA_PHY_BITRATE(PHY_BITRATE) ((PHY_BITRATE) <= DCMIPP_CSI_PHY_BT_2500)
#define IS_DCMIPP_CSI_DATA_TYPE_NB(DATA_TYPE_NB) ((DATA_TYPE_NB) <= MAX_DATATYPE_NB)

#define IS_DCMIPP_CSI_DATA_TYPE_FORMAT(DATA_TYPE_FORMAT) (((DATA_TYPE_FORMAT) == DCMIPP_CSI_DT_BPP6)  ||\
                                                          ((DATA_TYPE_FORMAT) == DCMIPP_CSI_DT_BPP7)  ||\
                                                          ((DATA_TYPE_FORMAT) == DCMIPP_CSI_DT_BPP8)  ||\
                                                          ((DATA_TYPE_FORMAT) == DCMIPP_CSI_DT_BPP10) ||\
                                                          ((DATA_TYPE_FORMAT) == DCMIPP_CSI_DT_BPP12) ||\
                                                          ((DATA_TYPE_FORMAT) == DCMIPP_CSI_DT_BPP14) ||\
                                                          ((DATA_TYPE_FORMAT) == DCMIPP_CSI_DT_BPP16))
#define IS_DCMIPP_CSI_DATA_CLASS(DATA_TYPE_CLASS)((DATA_TYPE_CLASS) <= 0x3FU)
#define IS_DCMIPP_CSI_COUNTER(COUNTER) (((COUNTER) == DCMIPP_CSI_COUNTER0) ||\
                                        ((COUNTER) == DCMIPP_CSI_COUNTER1) ||\
                                        ((COUNTER) == DCMIPP_CSI_COUNTER2) ||\
                                        ((COUNTER) == DCMIPP_CSI_COUNTER3))
#define IS_DCMIPP_CSI_LINE_COUNTER(LINE_COUNTER)((LINE_COUNTER) <= 0xFFFFU)
#define IS_DCMIPP_CSI_BYTE_COUNTER(BYTE_COUNTER)((BYTE_COUNTER) <= 0xFFFFU)

#define IS_DCMIPP_CSI_TIMER(TIMER) (((TIMER) == DCMIPP_CSI_TIMER0) ||\
                                    ((TIMER) == DCMIPP_CSI_TIMER1) ||\
                                    ((TIMER) == DCMIPP_CSI_TIMER2) ||\
                                    ((TIMER) == DCMIPP_CSI_TIMER3))
#define IS_DCMIPP_CSI_TIMER_START(TIMER_START) (((TIMER_START) == DCMIPP_CSI_TIMER_START_SOF)||\
                                                ((TIMER_START) == DCMIPP_CSI_TIMER_START_EOF))


#define IS_DCMIPP_FRAME_RATE(FRAME_RATE) (((FRAME_RATE) == DCMIPP_FRAME_RATE_ALL)      ||\
                                          ((FRAME_RATE) == DCMIPP_FRAME_RATE_1_OVER_2) ||\
                                          ((FRAME_RATE) == DCMIPP_FRAME_RATE_1_OVER_4) ||\
                                          ((FRAME_RATE) == DCMIPP_FRAME_RATE_1_OVER_8))
#define IS_DCMIPP_CLIENT(CLIENT) (((CLIENT) == DCMIPP_CLIENT1) ||\
                                  ((CLIENT) == DCMIPP_CLIENT2) ||\
                                  ((CLIENT) == DCMIPP_CLIENT3) ||\
                                  ((CLIENT) == DCMIPP_CLIENT4) ||\
                                  ((CLIENT) == DCMIPP_CLIENT5))

#define IS_DCMIPP_DPREG_END(DPREG_END) ((DPREG_END) <= 0x3FFU)
#define IS_DCMIPP_DPREG_START(DPREG_START) ((DPREG_START) <= 0x3FFU)

#define IS_DCMIPP_MAX_OUTSTANDING_TRANSACTIONS(OUTS_TRANS) (((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_NONE )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_2    )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_3    )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_4    )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_5    )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_6    )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_7    )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_8    )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_9    )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_10   )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_11   )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_12   )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_13   )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_14   )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_15   )||\
                                                            ((OUTS_TRANS) == DCMIPP_OUTSTANDING_TRANSACTION_16   ))

#define IS_DCMIPP_MEMORY_PAGE_SIZE(MEMORY_PAGE_SIZE) (((MEMORY_PAGE_SIZE) ==  DCMIPP_MEMORY_PAGE_SIZE_64BYTES)  ||\
                                                      ((MEMORY_PAGE_SIZE) ==  DCMIPP_MEMORY_PAGE_SIZE_128BYTES) ||\
                                                      ((MEMORY_PAGE_SIZE) ==  DCMIPP_MEMORY_PAGE_SIZE_256BYTES) ||\
                                                      ((MEMORY_PAGE_SIZE) ==  DCMIPP_MEMORY_PAGE_SIZE_512BYTES) ||\
                                                      ((MEMORY_PAGE_SIZE) ==  DCMIPP_MEMORY_PAGE_SIZE_1KBYTES)  ||\
                                                      ((MEMORY_PAGE_SIZE) ==  DCMIPP_MEMORY_PAGE_SIZE_2KBYTES)  ||\
                                                      ((MEMORY_PAGE_SIZE) ==  DCMIPP_MEMORY_PAGE_SIZE_4KBYTES)  ||\
                                                      ((MEMORY_PAGE_SIZE) ==  DCMIPP_MEMORY_PAGE_SIZE_8KBYTES))
#define IS_DCMIPP_TRAFFIC(TRAFFIC) (((TRAFFIC) == DCMIPP_TRAFFIC_BURST_SIZE_8BYTES ) ||\
                                    ((TRAFFIC) == DCMIPP_TRAFFIC_BURST_SIZE_16BYTES) ||\
                                    ((TRAFFIC) == DCMIPP_TRAFFIC_BURST_SIZE_32BYTES) ||\
                                    ((TRAFFIC) == DCMIPP_TRAFFIC_BURST_SIZE_64BYTES) ||\
                                    ((TRAFFIC) == DCMIPP_TRAFFIC_BURST_SIZE_128BYTES))
#define IS_DCMIPP_WLRU_RATIO(WLRU_RATIO) ((WLRU_RATIO)<= 0xFU)

#define IS_DCMIPP_CAPTURE_MODE(CAPTURE_MODE) (((CAPTURE_MODE) == DCMIPP_MODE_CONTINUOUS)||\
                                              ((CAPTURE_MODE) == DCMIPP_MODE_SNAPSHOT))

#define IS_DCMIPP_PIPE_CROP_AREA(CROP_AREA)(((CROP_AREA) == DCMIPP_POSITIVE_AREA) ||\
                                            ((CROP_AREA) == DCMIPP_NEGATIVE_AREA))

#define IS_DCMIPP_PIPE_CROP_HSIZE(CROP_HSIZE) (((CROP_HSIZE) >= 0x1U) && ((CROP_HSIZE) <= 0xFFFU))
#define IS_DCMIPP_PIPE_CROP_VSIZE(CROP_VSIZE) (((CROP_VSIZE) >= 0x1U) && ((CROP_VSIZE) <= 0xFFFU))
#define IS_DCMIPP_PIPE_CROP_VSTART(CROP_VSTART) ((CROP_VSTART) <= 0xFFFU)
#define IS_DCMIPP_PIPE_CROP_HSTART(CROP_HSTART) ((CROP_HSTART) <= 0xFFFU)

#define IS_DCMIPP_BYTE_SELECT_MODE(BYTE_SELECT) (((BYTE_SELECT) == DCMIPP_BSM_ALL)        ||\
                                                 ((BYTE_SELECT) == DCMIPP_BSM_DATA_OUT_2) ||\
                                                 ((BYTE_SELECT) == DCMIPP_BSM_BYTE_OUT_4) ||\
                                                 ((BYTE_SELECT) == DCMIPP_BSM_2BYTE_OUT_4))

#define IS_DCMIPP_BYTE_SELECT_START(BYTE_START)(((BYTE_START) == DCMIPP_OEBS_ODD) ||\
                                                ((BYTE_START) == DCMIPP_OEBS_EVEN))

#define IS_DCMIPP_LINE_SELECT_MODE(LINE_SELECT) (((LINE_SELECT) == DCMIPP_LSM_ALL) ||\
                                                 ((LINE_SELECT) == DCMIPP_LSM_ALTERNATE_2))

#define IS_DCMIPP_LINE_SELECT_START(LINE__START)(((LINE__START) == DCMIPP_OELS_ODD) ||\
                                                 ((LINE__START) == DCMIPP_OELS_EVEN))

#define IS_DCMIPP_MEMORY_ADDRESS(MEMORY_ADDRESS) (((MEMORY_ADDRESS) == DCMIPP_MEMORY_ADDRESS_0) ||\
                                                  ((MEMORY_ADDRESS) == DCMIPP_MEMORY_ADDRESS_1))

#define IS_DCMIPP_DATA_LIMIT(DATA_LIMIT) (((DATA_LIMIT) >=1U ) && ((DATA_LIMIT) <= 0xFFFFFFU))

#define IS_DCMIPP_PIPE_MULTILINE(MULTILINE) (((MULTILINE) == DCMIPP_MULTILINE_1_LINE )   ||\
                                             ((MULTILINE) == DCMIPP_MULTILINE_2_LINES)  ||\
                                             ((MULTILINE) == DCMIPP_MULTILINE_4_LINES)  ||\
                                             ((MULTILINE) == DCMIPP_MULTILINE_8_LINES)  ||\
                                             ((MULTILINE) == DCMIPP_MULTILINE_16_LINES) ||\
                                             ((MULTILINE) == DCMIPP_MULTILINE_32_LINES) ||\
                                             ((MULTILINE) == DCMIPP_MULTILINE_64_LINES) ||\
                                             ((MULTILINE) == DCMIPP_MULTILINE_128_LINES))
#define IS_DCMIPP_VRATIO(VRATIO) (((VRATIO) == DCMIPP_VDEC_ALL)     ||\
                                  ((VRATIO) == DCMIPP_VDEC_1_OUT_2) ||\
                                  ((VRATIO) == DCMIPP_VDEC_1_OUT_4) ||\
                                  ((VRATIO) == DCMIPP_VDEC_1_OUT_8))

#define IS_DCMIPP_HRATIO(HRATIO) (((HRATIO) == DCMIPP_HDEC_ALL)     ||\
                                  ((HRATIO) == DCMIPP_HDEC_1_OUT_2) ||\
                                  ((HRATIO) == DCMIPP_HDEC_1_OUT_4) ||\
                                  ((HRATIO) == DCMIPP_HDEC_1_OUT_8))

#define IS_DCMIPP_DOWSIZE_DIV_FACTOR(DIV_FACTOR) (((DIV_FACTOR)>=0x80U) && ((DIV_FACTOR)<= 0x3FFU))
#define IS_DCMIPP_DOWSIZE_RATIO(RATIO) ((RATIO)<= 0xFFFFU)
#define IS_DCMIPP_DOWSIZE_SIZE(SIZE) ((SIZE) <= 0xFFFU)

#define IS_DCMIPP_RAWBAYER2RGB_RAW_TYPE(RAW_TYPE) (((RAW_TYPE) == DCMIPP_RAWBAYER_RGGB) ||\
                                                   ((RAW_TYPE) == DCMIPP_RAWBAYER_GRBG) ||\
                                                   ((RAW_TYPE) == DCMIPP_RAWBAYER_GBRG) ||\
                                                   ((RAW_TYPE) == DCMIPP_RAWBAYER_BGGR))

#define IS_DCMIPP_RAWBAYER2RGB_STRENGTH(STRENGTH) (((STRENGTH) ==  DCMIPP_RAWBAYER_ALGO_NONE)       ||\
                                                   ((STRENGTH) ==  DCMIPP_RAWBAYER_ALGO_STRENGTH_3) ||\
                                                   ((STRENGTH) ==  DCMIPP_RAWBAYER_ALGO_STRENGTH_4) ||\
                                                   ((STRENGTH) ==  DCMIPP_RAWBAYER_ALGO_STRENGTH_6) ||\
                                                   ((STRENGTH) ==  DCMIPP_RAWBAYER_ALGO_STRENGTH_8) ||\
                                                   ((STRENGTH) ==  DCMIPP_RAWBAYER_ALGO_STRENGTH_12)||\
                                                   ((STRENGTH) ==  DCMIPP_RAWBAYER_ALGO_STRENGTH_16)||\
                                                   ((STRENGTH) ==  DCMIPP_RAWBAYER_ALGO_STRENGTH_24))

#define IS_DCMIPP_NB_FIRST_LINES(NB_FIRST_LINES) ((NB_FIRST_LINES) <= 7U)

#define IS_DCMIPP_NB_LAST_LINES(NB_LAST_LINES) ((NB_LAST_LINES) <= 0xFFFU)

#define IS_DCMIPP_BAD_PXL_REMOVAL_STRENGTH(STRENGTH) (((STRENGTH) ==  DCMIPP_BAD_PXL_REM_SRENGTH_0) ||\
                                                      ((STRENGTH) ==  DCMIPP_BAD_PXL_REM_SRENGTH_1) ||\
                                                      ((STRENGTH) ==  DCMIPP_BAD_PXL_REM_SRENGTH_2) ||\
                                                      ((STRENGTH) ==  DCMIPP_BAD_PXL_REM_SRENGTH_3) ||\
                                                      ((STRENGTH) ==  DCMIPP_BAD_PXL_REM_SRENGTH_4) ||\
                                                      ((STRENGTH) ==  DCMIPP_BAD_PXL_REM_SRENGTH_5) ||\
                                                      ((STRENGTH) ==  DCMIPP_BAD_PXL_REM_SRENGTH_6) ||\
                                                      ((STRENGTH) ==  DCMIPP_BAD_PXL_REM_SRENGTH_7))

#define IS_DCMIPP_ROI_COLOR(COLOR) ((COLOR) <= 3U)
#define IS_DCMIPP_ROI_SIZE(SIZE) ((SIZE) <= 3U)
#define IS_DCMIPP_ROI_START(START) ((START) <= 3U)
#define IS_DCMIPP_ROI_LINE_WIDTH(LINE_WIDTH) ((LINE_WIDTH) <= DCMIPP_LINE_WIDTH_8PXL)
#define IS_DCMIPP_ROI(REGION_OF_INTEREST) ((REGION_OF_INTEREST) <= DCMIPP_REGION_OF_INTEREST8)

#define IS_DCMIPP_ADDRESS_WRAP(ADDRESS_WRAP) (((ADDRESS_WRAP) == DCMIPP_WRAP_ADDRESS_1_LINE)   ||\
                                              ((ADDRESS_WRAP) == DCMIPP_WRAP_ADDRESS_2_LINES)  ||\
                                              ((ADDRESS_WRAP) == DCMIPP_WRAP_ADDRESS_4_LINES)  ||\
                                              ((ADDRESS_WRAP) == DCMIPP_WRAP_ADDRESS_8_LINES)  ||\
                                              ((ADDRESS_WRAP) == DCMIPP_WRAP_ADDRESS_16_LINES) ||\
                                              ((ADDRESS_WRAP) == DCMIPP_WRAP_ADDRESS_32_LINES) ||\
                                              ((ADDRESS_WRAP) == DCMIPP_WRAP_ADDRESS_64_LINES) ||\
                                              ((ADDRESS_WRAP) == DCMIPP_WRAP_ADDRESS_128_LINES))

#define IS_DCMIPP_OUTPUT_SAMPLES_TYPES(OUTPUT_SAMPLES_TYPES) (((OUTPUT_SAMPLES_TYPES) == DCMIPP_CLAMP_YUV) ||\
                                                              ((OUTPUT_SAMPLES_TYPES) == DCMIPP_CLAMP_RGB))
#define IS_DCMIPP_COLOR_CONVERSION_COEF(COLOR_CONV_CONF) ((COLOR_CONV_CONF) <= 0x7FF)

#define IS_DCMIPP_STAT_EXTRACTION_MODULE(MODULE) (((MODULE) == DCMIPP_STATEXT_MODULE1)||\
                                                  ((MODULE) == DCMIPP_STATEXT_MODULE2)||\
                                                  ((MODULE) == DCMIPP_STATEXT_MODULE3))

#define IS_DCMIPP_LUMINANCE(LUMINANCE) ((LUMINANCE) <= 0x3FU)

#define IS_DCMIPP_EXPOSURE_SHF(CHIFT) ((CHIFT) <= 0x7U)

#define IS_DCMIPP_STAT_EXTRACTION_BINS(BINS) (((BINS) ==  DCMIPP_STAT_EXT_BINS_MODE_LOWER_BINS)  ||\
                                              ((BINS) == DCMIPP_STAT_EXT_BINS_MODE_LOWMID_BINS) ||\
                                              ((BINS) == DCMIPP_STAT_EXT_BINS_MODE_UPMID_BINS ) ||\
                                              ((BINS) == DCMIPP_STAT_EXT_BINS_MODE_UP_BINS    ) ||\
                                              ((BINS) == DCMIPP_STAT_EXT_AVER_MODE_ALL_PIXELS ) ||\
                                              ((BINS) == DCMIPP_STAT_EXT_AVER_MODE_NOEXT16    ) ||\
                                              ((BINS) == DCMIPP_STAT_EXT_AVER_MODE_NOEXT32    ) ||\
                                              ((BINS) == DCMIPP_STAT_EXT_AVER_MODE_NOEXT64    ))

#define IS_DCMIPP_STAT_EXTRACTION_SOURCE(SOURCE) (((SOURCE) == DCMIPP_STAT_EXT_SOURCE_PRE_BLKLVL_R) ||\
                                                  ((SOURCE) == DCMIPP_STAT_EXT_SOURCE_PRE_BLKLVL_G) ||\
                                                  ((SOURCE) == DCMIPP_STAT_EXT_SOURCE_PRE_BLKLVL_B) ||\
                                                  ((SOURCE) == DCMIPP_STAT_EXT_SOURCE_PRE_BLKLVL_L) ||\
                                                  ((SOURCE) == DCMIPP_STAT_EXT_SOURCE_POST_DEMOS_R) ||\
                                                  ((SOURCE) == DCMIPP_STAT_EXT_SOURCE_POST_DEMOS_G) ||\
                                                  ((SOURCE) == DCMIPP_STAT_EXT_SOURCE_POST_DEMOS_B) ||\
                                                  ((SOURCE) == DCMIPP_STAT_EXT_SOURCE_POST_DEMOS_L))

#define IS_DCMIPP_STAT_EXTRACTION_MODE(MODE) (((MODE) == DCMIPP_STAT_EXT_MODE_AVERAGE) ||\
                                              ((MODE) == DCMIPP_STAT_EXT_MODE_BINS))

#define IS_DCMIPP_PIPE_STAT_EXTRACTION_START(START) ((START) <= 0xFFFU)
#define IS_DCMIPP_PIPE_STAT_EXTRACTION_SIZE(SIZE) ((SIZE) <= 0xFFFU)

/**
  * @}
  */

/**
  * @}
  */

#endif /* DCMIPP */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32N6xx_HAL_DCMIPP_H */

