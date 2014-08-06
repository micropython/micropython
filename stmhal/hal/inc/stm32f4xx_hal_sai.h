/**
  ******************************************************************************
  * @file    stm32f4xx_hal_sai.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of SAI HAL module.
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
#ifndef __STM32F4xx_HAL_SAI_H
#define __STM32F4xx_HAL_SAI_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"  

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup SAI
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

/** 
  * @brief  SAI Init Structure definition  
  */
typedef struct
{                                    
  uint32_t Protocol;        /*!< Specifies the SAI Block protocol.
                                 This parameter can be a value of @ref SAI_Block_Protocol             */
                                      
  uint32_t AudioMode;       /*!< Specifies the SAI Block audio Mode. 
                                 This parameter can be a value of @ref SAI_Block_Mode                 */

  uint32_t DataSize;        /*!< Specifies the SAI Block data size.
                                 This parameter can be a value of @ref SAI_Block_Data_Size            */

  uint32_t FirstBit;        /*!< Specifies whether data transfers start from MSB or LSB bit.
                                 This parameter can be a value of @ref SAI_Block_MSB_LSB_transmission */

  uint32_t ClockStrobing;   /*!< Specifies the SAI Block clock strobing edge sensitivity.
                                 This parameter can be a value of @ref SAI_Block_Clock_Strobing       */

  uint32_t Synchro;         /*!< Specifies SAI Block synchronization
                                 This parameter can be a value of @ref SAI_Block_Synchronization      */
 
  uint32_t OutputDrive;     /*!< Specifies when SAI Block outputs are driven.
                                 This parameter can be a value of @ref SAI_Block_Output_Drive
                                 @note this value has to be set before enabling the audio block  
                                       but after the audio block configuration.                       */

  uint32_t NoDivider;       /*!< Specifies whether master clock will be divided or not.
                                 This parameter can be a value of @ref SAI_Block_NoDivider
                                 @note: If bit NODIV in the SAI_xCR1 register is cleared, the frame length 
                                        should be aligned to a number equal to a power of 2, from 8 to 256.
                                        If bit NODIV in the SAI_xCR1 register is set, the frame length can 
                                        take any of the values without constraint since the input clock of 
                                        the audio block should be equal to the bit clock.
                                        There is no MCLK_x clock which can be output.                 */
  
  uint32_t FIFOThreshold;   /*!< Specifies SAI Block FIFO threshold.
                                 This parameter can be a value of @ref SAI_Block_Fifo_Threshold       */

  uint32_t ClockSource;     /*!< Specifies the SAI Block x Clock source.    
                                 This parameter can be a value of @ref SAI_Clock_Source 
                                 @note: If ClockSource is equal to SAI_CLKSource_Ext, the PLLI2S 
                                       and PLLSAI divisions factors will be ignored.                  */ 
                                     
  uint32_t AudioFrequency;  /*!< Specifies the audio frequency sampling.     
                                 This parameter can be a value of @ref SAI_Audio_Frequency            */
                                                                                                                              
}SAI_InitTypeDef;

/** 
  * @brief  SAI Block Frame Init structure definition  
  */
 
typedef struct
{

  uint32_t FrameLength;         /*!< Specifies the Frame length, the number of SCK clocks for each audio frame.
                                     This parameter must be a number between Min_Data = 8 and Max_Data = 256.
                                     @note: If master clock MCLK_x pin is declared as an output, the frame length
                                            should be aligned to a number equal to power of 2 in order to keep 
                                            in an audio frame, an integer number of MCLK pulses by bit Clock. */                                               
                                                                            
  uint32_t ActiveFrameLength;  /*!< Specifies the Frame synchronization active level length.
                                    This Parameter specifies the length in number of bit clock (SCK + 1)  
                                    of the active level of FS signal in audio frame.
                                    This parameter must be a number between Min_Data = 1 and Max_Data = 128   */
                                         
  uint32_t FSDefinition;       /*!< Specifies the Frame synchronization definition.
                                    This parameter can be a value of @ref SAI_Block_FS_Definition             */
                                         
  uint32_t FSPolarity;         /*!< Specifies the Frame synchronization Polarity.
                                    This parameter can be a value of @ref SAI_Block_FS_Polarity               */

  uint32_t FSOffset;           /*!< Specifies the Frame synchronization Offset.
                                    This parameter can be a value of @ref SAI_Block_FS_Offset                 */

}SAI_FrameInitTypeDef;

/**
  * @brief   SAI Block Slot Init Structure definition
  */    

typedef struct
{
  uint32_t FirstBitOffset;  /*!< Specifies the position of first data transfer bit in the slot.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 24 */

  uint32_t SlotSize;        /*!< Specifies the Slot Size.
                                 This parameter can be a value of @ref SAI_Block_Slot_Size              */

  uint32_t SlotNumber;      /*!< Specifies the number of slot in the audio frame.
                                 This parameter must be a number between Min_Data = 1 and Max_Data = 16 */

  uint32_t SlotActive;      /*!< Specifies the slots in audio frame that will be activated.
                                 This parameter can be a value of @ref SAI_Block_Slot_Active            */
}SAI_SlotInitTypeDef;

/** 
  * @brief  HAL State structures definition  
  */ 
typedef enum
{
  HAL_SAI_STATE_RESET      = 0x00,  /*!< SAI not yet initialized or disabled                */
  HAL_SAI_STATE_READY      = 0x01,  /*!< SAI initialized and ready for use                  */
  HAL_SAI_STATE_BUSY       = 0x02,  /*!< SAI internal process is ongoing                    */
  HAL_SAI_STATE_BUSY_TX    = 0x12,  /*!< Data transmission process is ongoing               */ 
  HAL_SAI_STATE_BUSY_RX    = 0x22,  /*!< Data reception process is ongoing                  */  
  HAL_SAI_STATE_TIMEOUT    = 0x03,  /*!< SAI timeout state                                  */
  HAL_SAI_STATE_ERROR      = 0x04   /*!< SAI error state                                    */
                                                                        
}HAL_SAI_StateTypeDef;

/** 
  * @brief  SAI handle Structure definition  
  */
typedef struct
{
  SAI_Block_TypeDef         *Instance;  /*!< SAI Blockx registers base address        */

  SAI_InitTypeDef           Init;       /*!< SAI communication parameters             */

  SAI_FrameInitTypeDef      FrameInit;  /*!< SAI Frame configuration parameters       */

  SAI_SlotInitTypeDef       SlotInit;   /*!< SAI Slot configuration parameters        */

  uint16_t                  *pTxBuffPtr; /*!< Pointer to SAI Tx transfer Buffer       */

  uint16_t                  TxXferSize;  /*!< SAI Tx transfer size                    */

  uint16_t                  TxXferCount; /*!< SAI Tx transfer counter                 */

  uint16_t                  *pRxBuffPtr; /*!< Pointer to SAI Rx transfer buffer       */

  uint16_t                  RxXferSize;  /*!< SAI Rx transfer size                    */

  uint16_t                  RxXferCount; /*!< SAI Rx transfer counter                 */

  DMA_HandleTypeDef         *hdmatx;     /*!< SAI Tx DMA handle parameters            */

  DMA_HandleTypeDef         *hdmarx;     /*!< SAI Rx DMA handle parameters            */

  HAL_LockTypeDef           Lock;        /*!< SAI locking object                      */

  __IO HAL_SAI_StateTypeDef State;       /*!< SAI communication state                 */

  __IO uint32_t             ErrorCode;   /*!< SAI Error code                          */
}SAI_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup SAI_Exported_Constants
  * @{
  */
/** @defgroup SAI Error Code 
  * @{
  */ 
#define HAL_SAI_ERROR_NONE      ((uint32_t)0x00000000)     /*!< No error             */
#define HAL_SAI_ERROR_OVR       ((uint32_t)0x00000001)     /*!< Overrun Error        */
#define HAL_SAI_ERROR_UDR       ((uint32_t)0x00000002)     /*!< Underrun error       */
#define HAL_SAI_ERROR_TIMEOUT   ((uint32_t)0x00000020)     /*!< Timeout error        */
/**
  * @}
  */

/** @defgroup SAI_Clock_Source
  * @{
  */
#define SAI_CLKSOURCE_PLLSAI             ((uint32_t)RCC_SAIACLKSOURCE_PLLSAI)
#define SAI_CLKSOURCE_PLLI2S             ((uint32_t)RCC_SAIACLKSOURCE_PLLI2S)
#define SAI_CLKSOURCE_EXT                ((uint32_t)RCC_SAIACLKSOURCE_EXT)

#define IS_SAI_CLK_SOURCE(SOURCE) (((SOURCE) == SAI_CLKSOURCE_PLLSAI) ||\
                                   ((SOURCE) == SAI_CLKSOURCE_PLLI2S) ||\
                                   ((SOURCE) == SAI_CLKSOURCE_EXT))
/**
  * @}
  */

/** @defgroup SAI_Audio_Frequency
  * @{
  */
#define SAI_AUDIO_FREQUENCY_192K          ((uint32_t)192000)
#define SAI_AUDIO_FREQUENCY_96K           ((uint32_t)96000)
#define SAI_AUDIO_FREQUENCY_48K           ((uint32_t)48000)
#define SAI_AUDIO_FREQUENCY_44K           ((uint32_t)44100)
#define SAI_AUDIO_FREQUENCY_32K           ((uint32_t)32000)
#define SAI_AUDIO_FREQUENCY_22K           ((uint32_t)22050)
#define SAI_AUDIO_FREQUENCY_16K           ((uint32_t)16000)
#define SAI_AUDIO_FREQUENCY_11K           ((uint32_t)11025)
#define SAI_AUDIO_FREQUENCY_8K            ((uint32_t)8000)    

#define IS_SAI_AUDIO_FREQUENCY(AUDIO) (((AUDIO) == SAI_AUDIO_FREQUENCY_192K) || ((AUDIO) == SAI_AUDIO_FREQUENCY_96K) || \
                                       ((AUDIO) == SAI_AUDIO_FREQUENCY_48K)  || ((AUDIO) == SAI_AUDIO_FREQUENCY_44K) || \
                                       ((AUDIO) == SAI_AUDIO_FREQUENCY_32K)  || ((AUDIO) == SAI_AUDIO_FREQUENCY_22K) || \
                                       ((AUDIO) == SAI_AUDIO_FREQUENCY_16K)  || ((AUDIO) == SAI_AUDIO_FREQUENCY_11K) || \
                                       ((AUDIO) == SAI_AUDIO_FREQUENCY_8K))
/**
  * @}
  */

/** @defgroup SAI_Block_Mode 
  * @{
  */
#define SAI_MODEMASTER_TX         ((uint32_t)0x00000000)
#define SAI_MODEMASTER_RX         ((uint32_t)0x00000001)
#define SAI_MODESLAVE_TX          ((uint32_t)0x00000002)
#define SAI_MODESLAVE_RX          ((uint32_t)0x00000003)
#define IS_SAI_BLOCK_MODE(MODE)    (((MODE) == SAI_MODEMASTER_TX) || \
                                    ((MODE) == SAI_MODEMASTER_RX) || \
                                    ((MODE) == SAI_MODESLAVE_TX)  || \
                                    ((MODE) == SAI_MODESLAVE_RX))
/**
  * @}
  */

/** @defgroup SAI_Block_Protocol 
  * @{
  */

#define SAI_FREE_PROTOCOL                 ((uint32_t)0x00000000)
#define SAI_AC97_PROTOCOL                 ((uint32_t)SAI_xCR1_PRTCFG_1)

#define IS_SAI_BLOCK_PROTOCOL(PROTOCOL) (((PROTOCOL) == SAI_FREE_PROTOCOL)  || \
                                         ((PROTOCOL) == SAI_AC97_PROTOCOL))
/**
  * @}
  */

/** @defgroup SAI_Block_Data_Size 
  * @{
  */
#define SAI_DATASIZE_8                   ((uint32_t)0x00000040)
#define SAI_DATASIZE_10                  ((uint32_t)0x00000060)
#define SAI_DATASIZE_16                  ((uint32_t)0x00000080)
#define SAI_DATASIZE_20                  ((uint32_t)0x000000A0)
#define SAI_DATASIZE_24                  ((uint32_t)0x000000C0)
#define SAI_DATASIZE_32                  ((uint32_t)0x000000E0)

#define IS_SAI_BLOCK_DATASIZE(DATASIZE) (((DATASIZE) == SAI_DATASIZE_8)  || \
                                         ((DATASIZE) == SAI_DATASIZE_10) || \
                                         ((DATASIZE) == SAI_DATASIZE_16) || \
                                         ((DATASIZE) == SAI_DATASIZE_20) || \
                                         ((DATASIZE) == SAI_DATASIZE_24) || \
                                         ((DATASIZE) == SAI_DATASIZE_32))
/**
  * @}
  */ 

/** @defgroup SAI_Block_MSB_LSB_transmission 
  * @{
  */

#define SAI_FIRSTBIT_MSB                  ((uint32_t)0x00000000)
#define SAI_FIRSTBIT_LSB                  ((uint32_t)SAI_xCR1_LSBFIRST)

#define IS_SAI_BLOCK_FIRST_BIT(BIT) (((BIT) == SAI_FIRSTBIT_MSB) || \
                                     ((BIT) == SAI_FIRSTBIT_LSB))
/**
  * @}
  */

/** @defgroup SAI_Block_Clock_Strobing 
  * @{
  */
#define SAI_CLOCKSTROBING_FALLINGEDGE     ((uint32_t)0x00000000)
#define SAI_CLOCKSTROBING_RISINGEDGE      ((uint32_t)SAI_xCR1_CKSTR)

#define IS_SAI_BLOCK_CLOCK_STROBING(CLOCK) (((CLOCK) == SAI_CLOCKSTROBING_FALLINGEDGE) || \
                                            ((CLOCK) == SAI_CLOCKSTROBING_RISINGEDGE))
/**
  * @}
  */

/** @defgroup SAI_Block_Synchronization 
  * @{
  */
#define SAI_ASYNCHRONOUS                  ((uint32_t)0x00000000)
#define SAI_SYNCHRONOUS                   ((uint32_t)SAI_xCR1_SYNCEN_0)

#define IS_SAI_BLOCK_SYNCHRO(SYNCHRO) (((SYNCHRO) == SAI_ASYNCHRONOUS) || \
                                       ((SYNCHRO) == SAI_SYNCHRONOUS))
/**
  * @}
  */ 

/** @defgroup SAI_Block_Output_Drive 
  * @{
  */
#define SAI_OUTPUTDRIVE_DISABLED          ((uint32_t)0x00000000)
#define SAI_OUTPUTDRIVE_ENABLED           ((uint32_t)SAI_xCR1_OUTDRIV)

#define IS_SAI_BLOCK_OUTPUT_DRIVE(DRIVE) (((DRIVE) == SAI_OUTPUTDRIVE_DISABLED) || \
                                          ((DRIVE) == SAI_OUTPUTDRIVE_ENABLED))
/**
  * @}
  */ 

/** @defgroup SAI_Block_NoDivider 
  * @{
  */
#define SAI_MASTERDIVIDER_ENABLED         ((uint32_t)0x00000000)
#define SAI_MASTERDIVIDER_DISABLED        ((uint32_t)SAI_xCR1_NODIV)

#define IS_SAI_BLOCK_NODIVIDER(NODIVIDER) (((NODIVIDER) == SAI_MASTERDIVIDER_ENABLED) || \
                                           ((NODIVIDER) == SAI_MASTERDIVIDER_DISABLED))
/**
  * @}
  */
  
/** @defgroup SAI_Block_Master_Divider 
  * @{
  */
#define IS_SAI_BLOCK_MASTER_DIVIDER(DIVIDER) ((DIVIDER) <= 15)
/**
  * @}
  */
  
/** @defgroup SAI_Block_Frame_Length 
  * @{
  */
#define IS_SAI_BLOCK_FRAME_LENGTH(LENGTH) ((8 <= (LENGTH)) && ((LENGTH) <= 256))
/**
  * @}
  */
    
/** @defgroup SAI_Block_Active_FrameLength 
  * @{
  */
#define IS_SAI_BLOCK_ACTIVE_FRAME(LENGTH) ((1 <= (LENGTH)) && ((LENGTH) <= 128))
/**
  * @}
  */

/** @defgroup SAI_Block_FS_Definition 
  * @{
  */
#define SAI_FS_STARTFRAME                 ((uint32_t)0x00000000)
#define SAI_FS_CHANNEL_IDENTIFICATION     ((uint32_t)SAI_xFRCR_FSDEF)

#define IS_SAI_BLOCK_FS_DEFINITION(DEFINITION) (((DEFINITION) == SAI_FS_STARTFRAME) || \
                                                ((DEFINITION) == SAI_FS_CHANNEL_IDENTIFICATION))
/**
  * @}
  */

/** @defgroup SAI_Block_FS_Polarity 
  * @{
  */
#define SAI_FS_ACTIVE_LOW                  ((uint32_t)0x00000000)
#define SAI_FS_ACTIVE_HIGH                 ((uint32_t)SAI_xFRCR_FSPO)

#define IS_SAI_BLOCK_FS_POLARITY(POLARITY) (((POLARITY) == SAI_FS_ACTIVE_LOW) || \
                                            ((POLARITY) == SAI_FS_ACTIVE_HIGH))
/**
  * @}
  */
            
/** @defgroup SAI_Block_FS_Offset 
  * @{
  */
#define SAI_FS_FIRSTBIT                   ((uint32_t)0x00000000)
#define SAI_FS_BEFOREFIRSTBIT             ((uint32_t)SAI_xFRCR_FSOFF)

#define IS_SAI_BLOCK_FS_OFFSET(OFFSET) (((OFFSET) == SAI_FS_FIRSTBIT) || \
                                        ((OFFSET) == SAI_FS_BEFOREFIRSTBIT))
/**
  * @}
  */
  
/** @defgroup SAI_Block_Slot_FirstBit_Offset
  * @{
  */
#define IS_SAI_BLOCK_FIRSTBIT_OFFSET(OFFSET) ((OFFSET) <= 24)
/**
  * @}
  */

  /** @defgroup SAI_Block_Slot_Size
  * @{
  */
#define SAI_SLOTSIZE_DATASIZE             ((uint32_t)0x00000000)  
#define SAI_SLOTSIZE_16B                  ((uint32_t)SAI_xSLOTR_SLOTSZ_0)
#define SAI_SLOTSIZE_32B                  ((uint32_t)SAI_xSLOTR_SLOTSZ_1)

#define IS_SAI_BLOCK_SLOT_SIZE(SIZE) (((SIZE) == SAI_SLOTSIZE_DATASIZE) || \
                                      ((SIZE) == SAI_SLOTSIZE_16B)      || \
                                      ((SIZE) == SAI_SLOTSIZE_32B))
/**
  * @}
  */

/** @defgroup SAI_Block_Slot_Number
  * @{
  */
#define IS_SAI_BLOCK_SLOT_NUMBER(NUMBER) ((1 <= (NUMBER)) && ((NUMBER) <= 16))
/**
  * @}
  */
  
/** @defgroup SAI_Block_Slot_Active
  * @{
  */
#define SAI_SLOT_NOTACTIVE           ((uint32_t)0x00000000)
#define SAI_SLOTACTIVE_0             ((uint32_t)0x00010000)
#define SAI_SLOTACTIVE_1             ((uint32_t)0x00020000)
#define SAI_SLOTACTIVE_2             ((uint32_t)0x00040000)
#define SAI_SLOTACTIVE_3             ((uint32_t)0x00080000)
#define SAI_SLOTACTIVE_4             ((uint32_t)0x00100000)
#define SAI_SLOTACTIVE_5             ((uint32_t)0x00200000)
#define SAI_SLOTACTIVE_6             ((uint32_t)0x00400000)
#define SAI_SLOTACTIVE_7             ((uint32_t)0x00800000)
#define SAI_SLOTACTIVE_8             ((uint32_t)0x01000000)
#define SAI_SLOTACTIVE_9             ((uint32_t)0x02000000)
#define SAI_SLOTACTIVE_10            ((uint32_t)0x04000000)
#define SAI_SLOTACTIVE_11            ((uint32_t)0x08000000)
#define SAI_SLOTACTIVE_12            ((uint32_t)0x10000000)
#define SAI_SLOTACTIVE_13            ((uint32_t)0x20000000)
#define SAI_SLOTACTIVE_14            ((uint32_t)0x40000000)
#define SAI_SLOTACTIVE_15            ((uint32_t)0x80000000)
#define SAI_SLOTACTIVE_ALL           ((uint32_t)0xFFFF0000)

#define IS_SAI_SLOT_ACTIVE(ACTIVE) ((ACTIVE) != 0)

/**
  * @}
  */

/** @defgroup SAI_Mono_Stereo_Mode
  * @{
  */
#define SAI_MONOMODE                      ((uint32_t)SAI_xCR1_MONO)
#define SAI_STREOMODE                     ((uint32_t)0x00000000)

#define IS_SAI_BLOCK_MONO_STREO_MODE(MODE) (((MODE) == SAI_MONOMODE) ||\
                                            ((MODE) == SAI_STREOMODE))
/**
  * @}
  */

/** @defgroup SAI_TRIState_Management
  * @{
  */
#define SAI_OUTPUT_NOTRELEASED              ((uint32_t)0x00000000)
#define SAI_OUTPUT_RELEASED                 ((uint32_t)SAI_xCR2_TRIS)

#define IS_SAI_BLOCK_TRISTATE_MANAGEMENT(STATE) (((STATE) == SAI_OUTPUT_NOTRELEASED) ||\
                                                 ((STATE) == SAI_OUTPUT_RELEASED))
/**
  * @}
  */

/** @defgroup SAI_Block_Fifo_Threshold 
  * @{
  */
#define SAI_FIFOTHRESHOLD_EMPTY  ((uint32_t)0x00000000)
#define SAI_FIFOTHRESHOLD_1QF    ((uint32_t)0x00000001)
#define SAI_FIFOTHRESHOLD_HF     ((uint32_t)0x00000002) 
#define SAI_FIFOTHRESHOLD_3QF    ((uint32_t)0x00000003)
#define SAI_FIFOTHRESHOLD_FULL   ((uint32_t)0x00000004)

#define IS_SAI_BLOCK_FIFO_THRESHOLD(THRESHOLD) (((THRESHOLD) == SAI_FIFOTHRESHOLD_EMPTY)   || \
                                                ((THRESHOLD) == SAI_FIFOTHRESHOLD_1QF)     || \
                                                ((THRESHOLD) == SAI_FIFOTHRESHOLD_HF)      || \
                                                ((THRESHOLD) == SAI_FIFOTHRESHOLD_3QF)     || \
                                                ((THRESHOLD) == SAI_FIFOTHRESHOLD_FULL))
/**
  * @}
  */
  
/** @defgroup SAI_Block_Companding_Mode
  * @{
  */
#define SAI_NOCOMPANDING                  ((uint32_t)0x00000000)
#define SAI_ULAW_1CPL_COMPANDING          ((uint32_t)0x00008000)
#define SAI_ALAW_1CPL_COMPANDING          ((uint32_t)0x0000C000)
#define SAI_ULAW_2CPL_COMPANDING          ((uint32_t)0x0000A000)
#define SAI_ALAW_2CPL_COMPANDING          ((uint32_t)0x0000E000)

#define IS_SAI_BLOCK_COMPANDING_MODE(MODE)    (((MODE) == SAI_NOCOMPANDING)         || \
                                               ((MODE) == SAI_ULAW_1CPL_COMPANDING) || \
                                               ((MODE) == SAI_ALAW_1CPL_COMPANDING) || \
                                               ((MODE) == SAI_ULAW_2CPL_COMPANDING) || \
                                               ((MODE) == SAI_ALAW_2CPL_COMPANDING))
/**
  * @}
  */

/** @defgroup SAI_Block_Mute_Value
  * @{
  */
#define SAI_ZERO_VALUE                     ((uint32_t)0x00000000)
#define SAI_LAST_SENT_VALUE                 ((uint32_t)SAI_xCR2_MUTEVAL)

#define IS_SAI_BLOCK_MUTE_VALUE(VALUE)    (((VALUE) == SAI_ZERO_VALUE)     || \
                                           ((VALUE) == SAI_LAST_SENT_VALUE))
/**
  * @}
  */

/** @defgroup SAI_Block_Mute_Frame_Counter
  * @{
  */ 
#define IS_SAI_BLOCK_MUTE_COUNTER(COUNTER) ((COUNTER) <= 63)
/**
  * @}
  */

/** @defgroup SAI_Block_Interrupts_Definition 
  * @{
  */
#define SAI_IT_OVRUDR                     ((uint32_t)SAI_xIMR_OVRUDRIE)
#define SAI_IT_MUTEDET                    ((uint32_t)SAI_xIMR_MUTEDETIE)
#define SAI_IT_WCKCFG                     ((uint32_t)SAI_xIMR_WCKCFGIE)
#define SAI_IT_FREQ                       ((uint32_t)SAI_xIMR_FREQIE)
#define SAI_IT_CNRDY                      ((uint32_t)SAI_xIMR_CNRDYIE)
#define SAI_IT_AFSDET                     ((uint32_t)SAI_xIMR_AFSDETIE)
#define SAI_IT_LFSDET                     ((uint32_t)SAI_xIMR_LFSDETIE)

#define IS_SAI_BLOCK_CONFIG_IT(IT) (((IT) == SAI_IT_OVRUDR)  || \
                                    ((IT) == SAI_IT_MUTEDET) || \
                                    ((IT) == SAI_IT_WCKCFG)  || \
                                    ((IT) == SAI_IT_FREQ)    || \
                                    ((IT) == SAI_IT_CNRDY)   || \
                                    ((IT) == SAI_IT_AFSDET)  || \
                                    ((IT) == SAI_IT_LFSDET))
/**
  * @}
  */

/** @defgroup SAI_Block_Flags_Definition 
  * @{
  */
#define SAI_FLAG_OVRUDR                   ((uint32_t)SAI_xSR_OVRUDR)
#define SAI_FLAG_MUTEDET                  ((uint32_t)SAI_xSR_MUTEDET)
#define SAI_FLAG_WCKCFG                   ((uint32_t)SAI_xSR_WCKCFG)
#define SAI_FLAG_FREQ                     ((uint32_t)SAI_xSR_FREQ)
#define SAI_FLAG_CNRDY                    ((uint32_t)SAI_xSR_CNRDY)
#define SAI_FLAG_AFSDET                   ((uint32_t)SAI_xSR_AFSDET)
#define SAI_FLAG_LFSDET                   ((uint32_t)SAI_xSR_LFSDET)

#define IS_SAI_BLOCK_GET_FLAG(FLAG) (((FLAG) == SAI_FLAG_OVRUDR)  || \
                                    ((FLAG) == SAI_FLAG_MUTEDET)  || \
                                    ((FLAG) == SAI_FLAG_WCKCFG)   || \
                                    ((FLAG) == SAI_FLAG_FREQ)     || \
                                    ((FLAG) == SAI_FLAG_CNRDY)    || \
                                    ((FLAG) == SAI_FLAG_AFSDET)   || \
                                    ((FLAG) == SAI_FLAG_LFSDET))
                                   
#define IS_SAI_BLOCK_CLEAR_FLAG(FLAG) (((FLAG) == SAI_FLAG_OVRUDR)  || \
                                       ((FLAG) == SAI_FLAG_MUTEDET) || \
                                       ((FLAG) == SAI_FLAG_WCKCFG)  || \
                                       ((FLAG) == SAI_FLAG_FREQ)    || \
                                       ((FLAG) == SAI_FLAG_CNRDY)   || \
                                       ((FLAG) == SAI_FLAG_AFSDET)  || \
                                       ((FLAG) == SAI_FLAG_LFSDET))
/**
  * @}
  */
  
/** @defgroup SAI_Block_Fifo_Status_Level 
  * @{
  */
#define SAI_FIFOStatus_Empty              ((uint32_t)0x00000000)
#define SAI_FIFOStatus_Less1QuarterFull   ((uint32_t)0x00010000)
#define SAI_FIFOStatus_1QuarterFull       ((uint32_t)0x00020000)
#define SAI_FIFOStatus_HalfFull           ((uint32_t)0x00030000) 
#define SAI_FIFOStatus_3QuartersFull      ((uint32_t)0x00040000)
#define SAI_FIFOStatus_Full               ((uint32_t)0x00050000)

#define IS_SAI_BLOCK_FIFO_STATUS(STATUS) (((STATUS) == SAI_FIFOStatus_Less1QuarterFull ) || \
                                          ((STATUS) == SAI_FIFOStatus_HalfFull)          || \
                                          ((STATUS) == SAI_FIFOStatus_1QuarterFull)      || \
                                          ((STATUS) == SAI_FIFOStatus_3QuartersFull)     || \
                                          ((STATUS) == SAI_FIFOStatus_Full)              || \
                                          ((STATUS) == SAI_FIFOStatus_Empty)) 
/**
  * @}
  */

  
/**
  * @}
  */
  
/* Exported macro ------------------------------------------------------------*/
/** @brief Reset SAI handle state
  * @param  __HANDLE__: specifies the SAI Handle.
  * @retval None
  */
#define __HAL_SAI_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_SAI_STATE_RESET)

/** @brief  Enable or disable the specified SAI interrupts.
  * @param  __HANDLE__: specifies the SAI Handle.
  * @param  __INTERRUPT__: specifies the interrupt source to enable or disable.
  *         This parameter can be one of the following values:
  *            @arg SAI_IT_OVRUDR: Overrun underrun interrupt enable                              
  *            @arg SAI_IT_MUTEDET: Mute detection interrupt enable                               
  *            @arg SAI_IT_WCKCFG: Wrong Clock Configuration interrupt enable                    
  *            @arg SAI_IT_FREQ: FIFO request interrupt enable                                  
  *            @arg SAI_IT_CNRDY: Codec not ready interrupt enable                               
  *            @arg SAI_IT_AFSDET: Anticipated frame synchronization detection interrupt enable   
  *            @arg SAI_IT_LFSDET: Late frame synchronization detection interrupt enabl
  * @retval None
  */
  
#define __HAL_SAI_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->IMR |= (__INTERRUPT__))
#define __HAL_SAI_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((__HANDLE__)->Instance->IMR &= (~(__INTERRUPT__)))
 
/** @brief  Check if the specified SAI interrupt source is enabled or disabled.
  * @param  __HANDLE__: specifies the SAI Handle.
  *         This parameter can be SAI where x: 1, 2, or 3 to select the SAI peripheral.
  * @param  __INTERRUPT__: specifies the SAI interrupt source to check.
  *         This parameter can be one of the following values:
  *            @arg SAI_IT_TXE: Tx buffer empty interrupt enable.
  *            @arg SAI_IT_RXNE: Rx buffer not empty interrupt enable.
  *            @arg SAI_IT_ERR: Error interrupt enable.
  * @retval The new state of __INTERRUPT__ (TRUE or FALSE).
  */
#define __HAL_SAI_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->IMR & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Check whether the specified SAI flag is set or not.
  * @param  __HANDLE__: specifies the SAI Handle.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg SAI_FLAG_OVRUDR: Overrun underrun flag.
  *            @arg SAI_FLAG_MUTEDET: Mute detection flag.
  *            @arg SAI_FLAG_WCKCFG: Wrong Clock Configuration flag.
  *            @arg SAI_FLAG_FREQ: FIFO request flag.
  *            @arg SAI_FLAG_CNRDY: Codec not ready flag.
  *            @arg SAI_FLAG_AFSDET: Anticipated frame synchronization detection flag.
  *            @arg SAI_FLAG_LFSDET: Late frame synchronization detection flag.  
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_SAI_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))

/** @brief  Clears the specified SAI pending flag.
  * @param  __HANDLE__: specifies the SAI Handle.
  * @param  __FLAG__: specifies the flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg SAI_FLAG_OVRUDR: Clear Overrun underrun  
  *            @arg SAI_FLAG_MUTEDET: Clear Mute detection 
  *            @arg SAI_FLAG_WCKCFG: Clear Wrong Clock Configuration  
  *            @arg SAI_FLAG_FREQ: Clear FIFO request   
  *            @arg SAI_FLAG_CNRDY: Clear Codec not ready
  *            @arg SAI_FLAG_AFSDET: Clear Anticipated frame synchronization detection
  *            @arg SAI_FLAG_LFSDET: Clear Late frame synchronization detection
  *   
  * @retval None
  */
#define __HAL_SAI_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->CLRFR = (__FLAG__))                                        

#define __HAL_SAI_ENABLE(__HANDLE__) ((__HANDLE__)->Instance->CR1 |=  SAI_xCR1_SAIEN)
#define __HAL_SAI_DISABLE(__HANDLE__) ((__HANDLE__)->Instance->CR1 &=  ~SAI_xCR1_SAIEN)
                                                                               
/* Exported functions --------------------------------------------------------*/

/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_SAI_Init(SAI_HandleTypeDef *hsai);
HAL_StatusTypeDef HAL_SAI_DeInit (SAI_HandleTypeDef *hsai);
void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai);
void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai);

/* I/O operation functions  *****************************************************/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_SAI_Transmit(SAI_HandleTypeDef *hsai, uint16_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SAI_Receive(SAI_HandleTypeDef *hsai, uint16_t *pData, uint16_t Size, uint32_t Timeout);

/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_SAI_Transmit_IT(SAI_HandleTypeDef *hsai, uint16_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SAI_Receive_IT(SAI_HandleTypeDef *hsai, uint16_t *pData, uint16_t Size);

/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_SAI_Transmit_DMA(SAI_HandleTypeDef *hsai, uint16_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SAI_Receive_DMA(SAI_HandleTypeDef *hsai, uint16_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SAI_DMAPause(SAI_HandleTypeDef *hsai);
HAL_StatusTypeDef HAL_SAI_DMAResume(SAI_HandleTypeDef *hsai);
HAL_StatusTypeDef HAL_SAI_DMAStop(SAI_HandleTypeDef *hsai);

/* SAI IRQHandler and Callbacks used in non blocking modes (Interrupt and DMA) */
void HAL_SAI_IRQHandler(SAI_HandleTypeDef *hsai);
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai);
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai);
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai);
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai);
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai);

/* Peripheral State functions  **************************************************/
HAL_SAI_StateTypeDef HAL_SAI_GetState(SAI_HandleTypeDef *hsai);
uint32_t HAL_SAI_GetError(SAI_HandleTypeDef *hsai);

#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
/**
  * @}
  */ 

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_SAI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
