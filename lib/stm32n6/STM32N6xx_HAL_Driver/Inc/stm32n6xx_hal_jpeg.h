/**
  ******************************************************************************
  * @file    stm32n6xx_hal_jpeg.h
  * @author  MCD Application Team
  * @brief   Header file of JPEG HAL module.
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
#ifndef STM32N6xx_HAL_JPEG_H
#define STM32N6xx_HAL_JPEG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

#if defined (JPEG)

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup JPEG
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup JPEG_Exported_Types JPEG Exported Types
  * @{
  */

/** @defgroup JPEG_Configuration_Structure_definition JPEG Configuration for encoding Structure definition
  * @brief  JPEG encoding configuration Structure definition
  * @{
  */
typedef struct
{
  uint32_t ColorSpace;               /*!< Image Color space : gray-scale, YCBCR, RGB or CMYK
                                           This parameter can be a value of @ref JPEG_ColorSpace */

  uint32_t ChromaSubsampling;        /*!< Chroma Subsampling in case of YCBCR or CMYK color space, 0-> 4:4:4 , 1-> 4:2:2, 2 -> 4:1:1, 3 -> 4:2:0
                                           This parameter can be a value of @ref JPEG_ChromaSubsampling */

  uint32_t ImageHeight;              /*!< Image height : number of lines */

  uint32_t ImageWidth;               /*!< Image width : number of pixels per line */

  uint32_t ImageQuality;             /*!< Quality of the JPEG encoding : from 1 to 100 */

} JPEG_ConfTypeDef;
/**
  * @}
  */

/** @defgroup HAL_JPEG_state_structure_definition HAL JPEG state structure definition
  * @brief  HAL JPEG State structure definition
  * @{
  */
typedef enum
{
  HAL_JPEG_STATE_RESET              = 0x00U,  /*!< JPEG not yet initialized or disabled  */
  HAL_JPEG_STATE_READY              = 0x01U,  /*!< JPEG initialized and ready for use    */
  HAL_JPEG_STATE_BUSY               = 0x02U,  /*!< JPEG internal processing is ongoing   */
  HAL_JPEG_STATE_BUSY_ENCODING      = 0x03U,  /*!< JPEG encoding processing is ongoing   */
  HAL_JPEG_STATE_BUSY_DECODING      = 0x04U,  /*!< JPEG decoding processing is ongoing   */
  HAL_JPEG_STATE_TIMEOUT            = 0x05U,  /*!< JPEG timeout state                    */
  HAL_JPEG_STATE_ERROR              = 0x06U   /*!< JPEG error state                      */
} HAL_JPEG_STATETypeDef;

/**
  * @}
  */


/** @defgroup JPEG_handle_Structure_definition JPEG handle Structure definition
  * @brief  JPEG handle Structure definition
  * @{
  */
#if (USE_HAL_JPEG_REGISTER_CALLBACKS == 1)
typedef struct __JPEG_HandleTypeDef
#else
typedef struct
#endif /* (USE_HAL_JPEG_REGISTER_CALLBACKS) */
{
  JPEG_TypeDef             *Instance;        /*!< JPEG peripheral register base address */

  JPEG_ConfTypeDef         Conf;             /*!< Current JPEG encoding/decoding parameters */

  uint8_t                  *pJpegInBuffPtr;  /*!< Pointer to JPEG processing (encoding, decoding,...) input buffer */

  uint8_t                  *pJpegOutBuffPtr; /*!< Pointer to JPEG processing (encoding, decoding,...) output buffer */

  __IO uint32_t            JpegInCount;      /*!< Internal Counter of input data */

  __IO uint32_t            JpegOutCount;     /*!< Internal Counter of output data */

  uint32_t                 InDataLength;     /*!< Input Buffer Length in Bytes */

  uint32_t                 OutDataLength;    /*!< Output Buffer Length in Bytes */

  DMA_HandleTypeDef        *hdmain;          /*!< JPEG In DMA handle parameters */

  DMA_HandleTypeDef        *hdmaout;         /*!< JPEG Out DMA handle parameters */

  uint8_t                  CustomQuanTable;  /*!< If set to 1 specify that user customized quantization tables are used */

  uint8_t                  *QuantTable0;     /*!< Basic Quantization Table for component 0 */

  uint8_t                  *QuantTable1;     /*!< Basic Quantization Table for component 1 */

  uint8_t                  *QuantTable2;     /*!< Basic Quantization Table for component 2 */

  uint8_t                  *QuantTable3;     /*!< Basic Quantization Table for component 3 */

  HAL_LockTypeDef          Lock;             /*!< JPEG locking object */

  __IO  HAL_JPEG_STATETypeDef State;         /*!< JPEG peripheral state */

  __IO  uint32_t           ErrorCode;        /*!< JPEG Error code */

  __IO uint32_t Context;                     /*!< JPEG Internal context */

#if (USE_HAL_JPEG_REGISTER_CALLBACKS == 1)
  void (*InfoReadyCallback)(struct __JPEG_HandleTypeDef *hjpeg,
                            JPEG_ConfTypeDef *pInfo);  /*!< JPEG Info ready callback      */
  void (*EncodeCpltCallback)(struct __JPEG_HandleTypeDef
                             *hjpeg);                          /*!< JPEG Encode complete callback */
  void (*DecodeCpltCallback)(struct __JPEG_HandleTypeDef
                             *hjpeg);                          /*!< JPEG Decode complete callback */
  void (*ErrorCallback)(struct __JPEG_HandleTypeDef
                        *hjpeg);                               /*!< JPEG Error callback           */
  void (*GetDataCallback)(struct __JPEG_HandleTypeDef *hjpeg,
                          uint32_t NbDecodedData);     /*!< JPEG Get Data callback        */
  void (*DataReadyCallback)(struct __JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut,
                            uint32_t OutDataLength);   /*!< JPEG Data ready callback */

  void (* MspInitCallback)(struct __JPEG_HandleTypeDef *hjpeg);                            /*!< JPEG Msp Init callback  */
  void (* MspDeInitCallback)(struct __JPEG_HandleTypeDef
                             *hjpeg);                          /*!< JPEG Msp DeInit callback  */


#endif /* (USE_HAL_JPEG_REGISTER_CALLBACKS) */


} JPEG_HandleTypeDef;
/**
  * @}
  */


#if (USE_HAL_JPEG_REGISTER_CALLBACKS == 1)
/** @defgroup HAL_JPEG_Callback_ID_enumeration_definition HAL JPEG Callback ID enumeration definition
  * @brief  HAL JPEG Callback ID enumeration definition
  * @{
  */
typedef enum
{
  HAL_JPEG_ENCODE_CPLT_CB_ID    = 0x01U,    /*!< JPEG Encode Complete callback ID */
  HAL_JPEG_DECODE_CPLT_CB_ID    = 0x02U,    /*!< JPEG Decode Complete callback ID */
  HAL_JPEG_ERROR_CB_ID          = 0x03U,    /*!< JPEG Error callback ID           */

  HAL_JPEG_MSPINIT_CB_ID        = 0x04U,    /*!< JPEG MspInit callback ID         */
  HAL_JPEG_MSPDEINIT_CB_ID      = 0x05U     /*!< JPEG MspDeInit callback ID       */

} HAL_JPEG_CallbackIDTypeDef;
/**
  * @}
  */

/** @defgroup HAL_JPEG_Callback_pointer_definition HAL JPEG Callback pointer definition
  * @brief  HAL JPEG Callback pointer definition
  * @{
  */
typedef  void (*pJPEG_CallbackTypeDef)(JPEG_HandleTypeDef *hjpeg);    /*!< pointer to a common JPEG callback function */
typedef  void (*pJPEG_InfoReadyCallbackTypeDef)(JPEG_HandleTypeDef *hjpeg,
                                                JPEG_ConfTypeDef *pInfo);  /*!< pointer to an Info ready JPEG callback function */
typedef  void (*pJPEG_GetDataCallbackTypeDef)(JPEG_HandleTypeDef *hjpeg,
                                              uint32_t NbDecodedData);     /*!< pointer to a Get data JPEG callback function */
typedef  void (*pJPEG_DataReadyCallbackTypeDef)(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut,
                                                uint32_t OutDataLength);  /*!< pointer to a Data ready JPEG callback function */
/**
  * @}
  */

#endif /* USE_HAL_JPEG_REGISTER_CALLBACKS */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup JPEG_Exported_Constants JPEG Exported Constants
  * @{
  */

/** @defgroup JPEG_Error_Code_definition JPEG Error Code definition
  * @brief  JPEG Error Code definition
  * @{
  */

#define  HAL_JPEG_ERROR_NONE        ((uint32_t)0x00000000U)    /*!< No error             */
#define  HAL_JPEG_ERROR_HUFF_TABLE  ((uint32_t)0x00000001U)    /*!< HUffman Table programming error */
#define  HAL_JPEG_ERROR_QUANT_TABLE ((uint32_t)0x00000002U)    /*!< Quantization Table programming error */
#define  HAL_JPEG_ERROR_DMA         ((uint32_t)0x00000004U)    /*!< DMA transfer error   */
#define  HAL_JPEG_ERROR_TIMEOUT     ((uint32_t)0x00000008U)    /*!< Timeout error        */
#if (USE_HAL_JPEG_REGISTER_CALLBACKS == 1)
#define  HAL_JPEG_ERROR_INVALID_CALLBACK ((uint32_t)0x00000010U)    /*!< Invalid Callback error  */
#endif /* USE_HAL_JPEG_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup JPEG_Quantization_Table_Size JPEG Quantization Table Size
  * @brief  JPEG Quantization Table Size
  * @{
  */
#define JPEG_QUANT_TABLE_SIZE  ((uint32_t)64U) /*!< JPEG Quantization Table Size in bytes  */
/**
  * @}
  */


/** @defgroup JPEG_ColorSpace JPEG ColorSpace
  * @brief  JPEG Color Space
  * @{
  */
#define JPEG_GRAYSCALE_COLORSPACE     ((uint32_t)0x00000000U)
#define JPEG_YCBCR_COLORSPACE         JPEG_CONFR1_COLORSPACE_0
#define JPEG_CMYK_COLORSPACE          JPEG_CONFR1_COLORSPACE


/**
  * @}
  */


/** @defgroup JPEG_ChromaSubsampling JPEG Chrominance Sampling
  * @brief  JPEG Chrominance Sampling
  * @{
  */
#define JPEG_444_SUBSAMPLING     ((uint32_t)0x00000000U)   /*!< Chroma Subsampling 4:4:4 */
#define JPEG_420_SUBSAMPLING     ((uint32_t)0x00000001U)   /*!< Chroma Subsampling 4:2:0 */
#define JPEG_422_SUBSAMPLING     ((uint32_t)0x00000002U)   /*!< Chroma Subsampling 4:2:2 */

/**
  * @}
  */

/** @defgroup JPEG_ImageQuality JPEG Image Quality
  * @brief  JPEG Min and Max Image Quality
  * @{
  */
#define JPEG_IMAGE_QUALITY_MIN     ((uint32_t)1U)     /*!< Minimum JPEG quality */
#define JPEG_IMAGE_QUALITY_MAX     ((uint32_t)100U)   /*!< Maximum JPEG quality */

/**
  * @}
  */

/** @defgroup JPEG_Interrupt_configuration_definition JPEG Interrupt configuration definition
  * @brief JPEG Interrupt definition
  * @{
  */
#define JPEG_IT_IFT     ((uint32_t)JPEG_CR_IFTIE)   /*!< Input FIFO Threshold Interrupt */
#define JPEG_IT_IFNF    ((uint32_t)JPEG_CR_IFNFIE)  /*!< Input FIFO Not Full Interrupt */
#define JPEG_IT_OFT     ((uint32_t)JPEG_CR_OFTIE)   /*!< Output FIFO Threshold Interrupt */
#define JPEG_IT_OFNE    ((uint32_t)JPEG_CR_OFTIE)   /*!< Output FIFO Not Empty Interrupt */
#define JPEG_IT_EOC     ((uint32_t)JPEG_CR_EOCIE)   /*!< End of Conversion Interrupt */
#define JPEG_IT_HPD     ((uint32_t)JPEG_CR_HPDIE)   /*!< Header Parsing Done Interrupt */
/**
  * @}
  */

/** @defgroup JPEG_Flag_definition JPEG Flag definition
  * @brief JPEG Flags definition
  * @{
  */
#define JPEG_FLAG_IFTF     ((uint32_t)JPEG_SR_IFTF)   /*!< Input FIFO is not full and is below its threshold flag */
#define JPEG_FLAG_IFNFF    ((uint32_t)JPEG_SR_IFNFF)  /*!< Input FIFO Not Full Flag, a data can be written */
#define JPEG_FLAG_OFTF     ((uint32_t)JPEG_SR_OFTF)   /*!< Output FIFO is not empty and has reach its threshold */
#define JPEG_FLAG_OFNEF    ((uint32_t)JPEG_SR_OFNEF)  /*!< Output FIFO is not empty, a data is available  */
#define JPEG_FLAG_EOCF     ((uint32_t)JPEG_SR_EOCF)   /*!< JPEG Codec core has finished the encoding or the decoding process and than last data has been sent to the output FIFO  */
#define JPEG_FLAG_HPDF     ((uint32_t)JPEG_SR_HPDF)   /*!< JPEG Codec has finished the parsing of the headers and the internal registers have been updated  */
#define JPEG_FLAG_COF      ((uint32_t)JPEG_SR_COF)    /*!< JPEG Codec operation on going  flag*/

#define JPEG_FLAG_ALL      ((uint32_t)0x000000FEU)     /*!< JPEG Codec All previous flag*/
/**
  * @}
  */

/** @defgroup JPEG_PROCESS_PAUSE_RESUME_definition JPEG Process Pause Resume definition
  * @brief JPEG process pause, resume definition
  * @{
  */
#define JPEG_PAUSE_RESUME_INPUT          ((uint32_t)0x00000001U)     /*!< Pause/Resume Input FIFO Xfer*/
#define JPEG_PAUSE_RESUME_OUTPUT         ((uint32_t)0x00000002U)     /*!< Pause/Resume Output FIFO Xfer*/
#define JPEG_PAUSE_RESUME_INPUT_OUTPUT   ((uint32_t)0x00000003U)     /*!< Pause/Resume Input and Output FIFO Xfer*/
/**
  * @}
  */

/**
  * @}
  */
/* Exported macro ------------------------------------------------------------*/

/** @defgroup JPEG_Exported_Macros JPEG Exported Macros
  * @{
  */

/** @brief Reset JPEG handle state
  * @param  __HANDLE__ specifies the JPEG handle.
  * @retval None
  */
#if (USE_HAL_JPEG_REGISTER_CALLBACKS == 1)
#define __HAL_JPEG_RESET_HANDLE_STATE(__HANDLE__) do{\
                                                      (__HANDLE__)->State = HAL_JPEG_STATE_RESET;\
                                                      (__HANDLE__)->MspInitCallback = NULL;\
                                                      (__HANDLE__)->MspDeInitCallback = NULL;\
                                                      }while(0)
#else
#define __HAL_JPEG_RESET_HANDLE_STATE(__HANDLE__) ( (__HANDLE__)->State = HAL_JPEG_STATE_RESET)
#endif /* USE_HAL_JPEG_REGISTER_CALLBACKS */


/**
  * @brief  Enable the JPEG peripheral.
  * @param  __HANDLE__ specifies the JPEG handle.
  * @retval None
  */
#define __HAL_JPEG_ENABLE(__HANDLE__)  ((__HANDLE__)->Instance->CR |=  JPEG_CR_JCEN)

/**
  * @brief Disable the JPEG peripheral.
  * @param  __HANDLE__ specifies the JPEG handle.
  * @retval None
  */
#define __HAL_JPEG_DISABLE(__HANDLE__) ((__HANDLE__)->Instance->CR &=  ~JPEG_CR_JCEN)


/**
  * @brief  Check the specified JPEG status flag.
  * @param  __HANDLE__ specifies the JPEG handle.
  * @param  __FLAG__  specifies the flag to check
  *         This parameter can be one of the following values:
  *         @arg JPEG_FLAG_IFTF  : The input FIFO is not full and is below its threshold flag
  *         @arg JPEG_FLAG_IFNFF : The input FIFO Not Full Flag, a data can be written
  *         @arg JPEG_FLAG_OFTF  : The output FIFO is not empty and has reach its threshold
  *         @arg JPEG_FLAG_OFNEF : The output FIFO is not empty, a data is available
  *         @arg JPEG_FLAG_EOCF  : JPEG Codec core has finished the encoding or the decoding process
  *                                and than last data has been sent to the output FIFO
  *         @arg JPEG_FLAG_HPDF  : JPEG Codec has finished the parsing of the headers
  *                                and the internal registers have been updated
  *         @arg JPEG_FLAG_COF   : JPEG Codec operation on going  flag
  *
  * @retval __HAL_JPEG_GET_FLAG : returns The new state of __FLAG__ (TRUE or FALSE)
  */

#define __HAL_JPEG_GET_FLAG(__HANDLE__,__FLAG__)  (((__HANDLE__)->Instance->SR & (__FLAG__)))

/**
  * @brief  Clear the specified JPEG status flag.
  * @param  __HANDLE__ specifies the JPEG handle.
  * @param  __FLAG__  specifies the flag to clear
  *         This parameter can be one of the following values:
  *         @arg JPEG_FLAG_EOCF  : JPEG Codec core has finished the encoding or the decoding process
  *                                and than last data has been sent to the output FIFO
  *         @arg JPEG_FLAG_HPDF  : JPEG Codec has finished the parsing of the headers
  * @retval None
  */

#define __HAL_JPEG_CLEAR_FLAG(__HANDLE__,__FLAG__)  (((__HANDLE__)->Instance->CFR |= ((__FLAG__) &\
                                                      (JPEG_FLAG_EOCF | JPEG_FLAG_HPDF))))


/**
  * @brief  Enable Interrupt.
  * @param   __HANDLE__ specifies the JPEG handle.
  * @param  __INTERRUPT__  specifies the interrupt to enable
  *         This parameter can be one of the following values:
  *         @arg JPEG_IT_IFT   : Input FIFO Threshold Interrupt
  *         @arg JPEG_IT_IFNF  : Input FIFO Not Full Interrupt
  *         @arg JPEG_IT_OFT   : Output FIFO Threshold Interrupt
  *         @arg JPEG_IT_OFNE  : Output FIFO Not empty Interrupt
  *         @arg JPEG_IT_EOC   : End of Conversion Interrupt
  *         @arg JPEG_IT_HPD   : Header Parsing Done Interrupt
  *
  * @retval No return
  */
#define __HAL_JPEG_ENABLE_IT(__HANDLE__,__INTERRUPT__)  ((__HANDLE__)->Instance->CR |= (__INTERRUPT__) )

/**
  * @brief  Disable Interrupt.
  * @param   __HANDLE__ specifies the JPEG handle.
  * @param  __INTERRUPT__ specifies the interrupt to disable
  *         This parameter can be one of the following values:
  *         @arg JPEG_IT_IFT   : Input FIFO Threshold Interrupt
  *         @arg JPEG_IT_IFNF  : Input FIFO Not Full Interrupt
  *         @arg JPEG_IT_OFT   : Output FIFO Threshold Interrupt
  *         @arg JPEG_IT_OFNE  : Output FIFO Not empty Interrupt
  *         @arg JPEG_IT_EOC   : End of Conversion Interrupt
  *         @arg JPEG_IT_HPD   : Header Parsing Done Interrupt
  *
  * @note    To disable an IT we must use MODIFY_REG macro to avoid writing "1" to the FIFO flush bits
  *          located in the same IT enable register (CR register).
  * @retval  No return
  */
#define __HAL_JPEG_DISABLE_IT(__HANDLE__,__INTERRUPT__) MODIFY_REG((__HANDLE__)->Instance->CR, (__INTERRUPT__), 0UL)


/**
  * @brief  Get Interrupt state.
  * @param   __HANDLE__ specifies the JPEG handle.
  * @param  __INTERRUPT__  specifies the interrupt to check
  *         This parameter can be one of the following values:
  *         @arg JPEG_IT_IFT   : Input FIFO Threshold Interrupt
  *         @arg JPEG_IT_IFNF  : Input FIFO Not Full Interrupt
  *         @arg JPEG_IT_OFT   : Output FIFO Threshold Interrupt
  *         @arg JPEG_IT_OFNE  : Output FIFO Not empty Interrupt
  *         @arg JPEG_IT_EOC   : End of Conversion Interrupt
  *         @arg JPEG_IT_HPD   : Header Parsing Done Interrupt
  *
  * @retval returns The new state of __INTERRUPT__ (Enabled or disabled)
  */
#define __HAL_JPEG_GET_IT_SOURCE(__HANDLE__,__INTERRUPT__)     ((__HANDLE__)->Instance->CR & (__INTERRUPT__))

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup JPEG_Exported_Functions
  * @{
  */

/** @addtogroup JPEG_Exported_Functions_Group1
  * @{
  */
/* Initialization/de-initialization functions  ********************************/
HAL_StatusTypeDef HAL_JPEG_Init(JPEG_HandleTypeDef *hjpeg);
HAL_StatusTypeDef HAL_JPEG_DeInit(JPEG_HandleTypeDef *hjpeg);
void HAL_JPEG_MspInit(JPEG_HandleTypeDef *hjpeg);
void HAL_JPEG_MspDeInit(JPEG_HandleTypeDef *hjpeg);

#if (USE_HAL_JPEG_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_JPEG_RegisterCallback(JPEG_HandleTypeDef *hjpeg, HAL_JPEG_CallbackIDTypeDef CallbackID,
                                            pJPEG_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_JPEG_UnRegisterCallback(JPEG_HandleTypeDef *hjpeg, HAL_JPEG_CallbackIDTypeDef CallbackID);

HAL_StatusTypeDef HAL_JPEG_RegisterInfoReadyCallback(JPEG_HandleTypeDef *hjpeg,
                                                     pJPEG_InfoReadyCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_JPEG_UnRegisterInfoReadyCallback(JPEG_HandleTypeDef *hjpeg);

HAL_StatusTypeDef HAL_JPEG_RegisterGetDataCallback(JPEG_HandleTypeDef *hjpeg, pJPEG_GetDataCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_JPEG_UnRegisterGetDataCallback(JPEG_HandleTypeDef *hjpeg);

HAL_StatusTypeDef HAL_JPEG_RegisterDataReadyCallback(JPEG_HandleTypeDef *hjpeg,
                                                     pJPEG_DataReadyCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_JPEG_UnRegisterDataReadyCallback(JPEG_HandleTypeDef *hjpeg);

#endif /* USE_HAL_JPEG_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @addtogroup JPEG_Exported_Functions_Group2
  * @{
  */
/* Encoding/Decoding Configuration functions  ********************************/
HAL_StatusTypeDef HAL_JPEG_ConfigEncoding(JPEG_HandleTypeDef *hjpeg, const JPEG_ConfTypeDef *pConf);
HAL_StatusTypeDef HAL_JPEG_GetInfo(JPEG_HandleTypeDef *hjpeg, JPEG_ConfTypeDef *pInfo);
HAL_StatusTypeDef HAL_JPEG_EnableHeaderParsing(JPEG_HandleTypeDef *hjpeg);
HAL_StatusTypeDef HAL_JPEG_DisableHeaderParsing(JPEG_HandleTypeDef *hjpeg);
HAL_StatusTypeDef HAL_JPEG_SetUserQuantTables(JPEG_HandleTypeDef *hjpeg, uint8_t *QTable0, uint8_t *QTable1,
                                              uint8_t *QTable2, uint8_t *QTable3);

/**
  * @}
  */

/** @addtogroup JPEG_Exported_Functions_Group3
  * @{
  */
/* JPEG processing functions  **************************************/
HAL_StatusTypeDef  HAL_JPEG_Encode(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataInMCU, uint32_t InDataLength,
                                   uint8_t *pDataOut, uint32_t OutDataLength, uint32_t Timeout);
HAL_StatusTypeDef  HAL_JPEG_Decode(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataIn, uint32_t InDataLength,
                                   uint8_t *pDataOutMCU, uint32_t OutDataLength, uint32_t Timeout);
HAL_StatusTypeDef  HAL_JPEG_Encode_IT(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataInMCU, uint32_t InDataLength,
                                      uint8_t *pDataOut, uint32_t OutDataLength);
HAL_StatusTypeDef  HAL_JPEG_Decode_IT(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataIn, uint32_t InDataLength,
                                      uint8_t *pDataOutMCU, uint32_t OutDataLength);
HAL_StatusTypeDef  HAL_JPEG_Encode_DMA(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataInMCU, uint32_t InDataLength,
                                       uint8_t *pDataOut, uint32_t OutDataLength);
HAL_StatusTypeDef  HAL_JPEG_Decode_DMA(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataIn, uint32_t InDataLength,
                                       uint8_t *pDataOutMCU, uint32_t OutDataLength);
HAL_StatusTypeDef  HAL_JPEG_Pause(JPEG_HandleTypeDef *hjpeg, uint32_t XferSelection);
HAL_StatusTypeDef  HAL_JPEG_Resume(JPEG_HandleTypeDef *hjpeg, uint32_t XferSelection);
void HAL_JPEG_ConfigInputBuffer(JPEG_HandleTypeDef *hjpeg, uint8_t *pNewInputBuffer, uint32_t InDataLength);
void HAL_JPEG_ConfigOutputBuffer(JPEG_HandleTypeDef *hjpeg, uint8_t *pNewOutputBuffer, uint32_t OutDataLength);
HAL_StatusTypeDef HAL_JPEG_Abort(JPEG_HandleTypeDef *hjpeg);

/**
  * @}
  */

/** @addtogroup JPEG_Exported_Functions_Group4
  * @{
  */
/* JPEG Decode/Encode callback functions  ********************************************************/
void HAL_JPEG_InfoReadyCallback(JPEG_HandleTypeDef *hjpeg, JPEG_ConfTypeDef *pInfo);
void HAL_JPEG_EncodeCpltCallback(JPEG_HandleTypeDef *hjpeg);
void HAL_JPEG_DecodeCpltCallback(JPEG_HandleTypeDef *hjpeg);
void HAL_JPEG_ErrorCallback(JPEG_HandleTypeDef *hjpeg);
void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbDecodedData);
void HAL_JPEG_DataReadyCallback(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength);

/**
  * @}
  */

/** @addtogroup JPEG_Exported_Functions_Group5
  * @{
  */
/* JPEG IRQ handler management  ******************************************************/
void HAL_JPEG_IRQHandler(JPEG_HandleTypeDef *hjpeg);

/**
  * @}
  */

/** @addtogroup JPEG_Exported_Functions_Group6
  * @{
  */
/* Peripheral State and Error functions  ************************************************/
HAL_JPEG_STATETypeDef  HAL_JPEG_GetState(const JPEG_HandleTypeDef *hjpeg);
uint32_t               HAL_JPEG_GetError(const JPEG_HandleTypeDef *hjpeg);

/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/** @defgroup JPEG_Private_Types JPEG Private Types
  * @{
  */

/**
  * @}
  */

/* Private defines -----------------------------------------------------------*/
/** @defgroup JPEG_Private_Defines JPEG Private Defines
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup JPEG_Private_Variables JPEG Private Variables
  * @{
  */

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup JPEG_Private_Constants JPEG Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup JPEG_Private_Macros JPEG Private Macros
  * @{
  */

#define IS_JPEG_CHROMASUBSAMPLING(SUBSAMPLING) (((SUBSAMPLING) == JPEG_444_SUBSAMPLING) || \
                                                ((SUBSAMPLING) == JPEG_420_SUBSAMPLING) || \
                                                ((SUBSAMPLING) == JPEG_422_SUBSAMPLING))

#define IS_JPEG_IMAGE_QUALITY(NUMBER) (((NUMBER) >= JPEG_IMAGE_QUALITY_MIN) && ((NUMBER) <= JPEG_IMAGE_QUALITY_MAX))

#define IS_JPEG_COLORSPACE(COLORSPACE) (((COLORSPACE) == JPEG_GRAYSCALE_COLORSPACE) || \
                                        ((COLORSPACE) == JPEG_YCBCR_COLORSPACE)     || \
                                        ((COLORSPACE) == JPEG_CMYK_COLORSPACE))

#define IS_JPEG_PAUSE_RESUME_STATE(VALUE) (((VALUE) == JPEG_PAUSE_RESUME_INPUT) || \
                                           ((VALUE) == JPEG_PAUSE_RESUME_OUTPUT)|| \
                                           ((VALUE) == JPEG_PAUSE_RESUME_INPUT_OUTPUT))

/**
  * @}
  */

/* Private functions prototypes ----------------------------------------------*/
/** @defgroup JPEG_Private_Functions_Prototypes JPEG Private Functions Prototypes
  * @{
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup JPEG_Private_Functions JPEG Private Functions
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

#endif /* JPEG */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_JPEG_H */

