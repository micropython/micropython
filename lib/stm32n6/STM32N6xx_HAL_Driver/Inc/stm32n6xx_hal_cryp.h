/**
  ******************************************************************************
  * @file    stm32n6xx_hal_cryp.h
  * @author  MCD Application Team
  * @brief   Header file of CRYP HAL module.
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
#ifndef STM32N6xx_HAL_CRYP_H
#define STM32N6xx_HAL_CRYP_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(USE_HAL_SAES_ONLY) && (USE_HAL_SAES_ONLY == 1U)
#if !defined(USE_HAL_CRYP_ONLY)
#define USE_HAL_CRYP_ONLY       0U
#elif (USE_HAL_CRYP_ONLY == 1U)
#error ' USE_HAL_CRYP_ONLY and USE_HAL_SAES_ONLY cannot be set both to 1U '
#endif /* defined (USE_HAL_CRYP_ONLY) */
#endif /* defined (USE_HAL_SAES_ONLY) */

#if defined(USE_HAL_CRYP_ONLY) && (USE_HAL_CRYP_ONLY == 1U)
#if !defined(USE_HAL_SAES_ONLY)
#define USE_HAL_SAES_ONLY       0U
#elif (USE_HAL_SAES_ONLY == 1U)
#error ' USE_HAL_CRYP_ONLY and USE_HAL_SAES_ONLY cannot be set both to 1U '
#endif /* defined (USE_HAL_SAES_ONLY) */
#endif /* defined (USE_HAL_CRYP_ONLY) */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */
#if defined (CRYP)
/** @addtogroup CRYP
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup CRYP_Exported_Types CRYP Exported Types
  * @{
  */

/**
  * @brief CRYP Init Structure definition
  */

typedef struct
{
  uint32_t DataType;                   /*!< 32-bit data, 16-bit data, 8-bit data or 1-bit string.
                                        This parameter can be a value of @ref CRYP_Data_Type */
  uint32_t KeySize;                    /*!< Used only in AES mode : 128, 192 or 256 bit key length in CRYP1.
                                        This parameter can be a value of @ref CRYP_Key_Size */
  uint32_t *pKey;                      /*!< The key used for encryption/decryption */
  uint32_t *pInitVect;                 /*!< The initialization vector used also as initialization
                                         counter in CTR mode */
  uint32_t Algorithm;                  /*!< AES Algorithm ECB/CBC/CTR/GCM or CCM
                                        This parameter can be a value of @ref CRYP_CR_ALGOMODE */
  uint32_t *Header;                    /*!< used only in AES GCM and CCM Algorithm for authentication,
                                        GCM : also known as Additional Authentication Data
                                        CCM : named B1 composed of the associated data length and Associated Data. */
  uint32_t HeaderSize;                 /*!< The size of header buffer in word  */
  uint32_t *B0;                        /*!< B0 is first authentication block used only in AES CCM mode */
  uint32_t DataWidthUnit;              /*!< Data With Unit, this parameter can be value of @ref CRYP_Data_Width_Unit*/
  uint32_t HeaderWidthUnit;            /*!< Header Width Unit, this parameter can be value
                                            of @ref CRYP_Header_Width_Unit */
  uint32_t KeyIVConfigSkip;            /*!< CRYP peripheral Key and IV configuration skip,
                                            to configure Key and Initialization
                                            Vector only once and to skip configuration for consecutive processing.
                                        This parameter can be a value of @ref CRYP_Configuration_Skip */
  uint32_t KeyMode;                    /*!< Key mode selection, this parameter can be a value of @ref CRYP_Key_Mode */
  uint32_t KeySelect;                  /*!< Only for SAES : Key selection, this parameter can be a value
                                            of @ref CRYP_Key_Select */
  uint32_t KeyProtection;              /*!< Only for SAES : Key protection, this parameter can be a value of @ref CRYP_Key_Protection */

} CRYP_ConfigTypeDef;


/**
  * @brief  CRYP State Structure definition
  */

typedef enum
{
  HAL_CRYP_STATE_RESET             = 0x00U,  /*!< CRYP not yet initialized or disabled       */
  HAL_CRYP_STATE_READY             = 0x01U,  /*!< CRYP initialized and ready for use         */
  HAL_CRYP_STATE_BUSY              = 0x02U,  /*!< CRYP BUSY, internal processing is ongoing  */
#if (USE_HAL_CRYP_SUSPEND_RESUME == 1U)
  HAL_CRYP_STATE_SUSPENDED         = 0x03U,  /*!< CRYP suspended                             */
#endif /* USE_HAL_CRYP_SUSPEND_RESUME */
} HAL_CRYP_STATETypeDef;


#if (USE_HAL_CRYP_SUSPEND_RESUME == 1U)
/**
  * @brief HAL CRYP mode suspend definitions
  */

typedef enum
{
  HAL_CRYP_SUSPEND_NONE            = 0x00U,    /*!< CRYP processing suspension not requested */
  HAL_CRYP_SUSPEND                 = 0x01U     /*!< CRYP processing suspension requested     */
} HAL_SuspendTypeDef;
#endif /* USE_HAL_CRYP_SUSPEND_RESUME */

/**
  * @brief  CRYP handle Structure definition
  */

#if (USE_HAL_CRYP_REGISTER_CALLBACKS == 1U)
typedef struct __CRYP_HandleTypeDef
#else
typedef struct
#endif /* (USE_HAL_CRYP_REGISTER_CALLBACKS) */
{
  void                              *Instance;        /*!< CRYP or SAES registers base address */

  CRYP_ConfigTypeDef                Init;             /*!< CRYP required parameters */

  uint32_t                          *pCrypInBuffPtr;  /*!< Pointer to CRYP processing (encryption, decryption,...)
                                                           buffer */

  uint32_t                          *pCrypOutBuffPtr; /*!< Pointer to CRYP processing (encryption, decryption,...)
                                                           buffer */

  __IO uint16_t                     CrypHeaderCount;  /*!< Counter of header data */

  __IO uint16_t                     CrypInCount;      /*!< Counter of input data */

  __IO uint16_t                     CrypOutCount;     /*!< Counter of output data */

  uint16_t                          Size;             /*!< length of input data in word or in byte,
                                                           according to DataWidthUnit */

  uint32_t                          Phase;            /*!< CRYP peripheral phase */

  DMA_HandleTypeDef                 *hdmain;          /*!< CRYP In DMA handle parameters */

  DMA_HandleTypeDef                 *hdmaout;         /*!< CRYP Out DMA handle parameters */

  HAL_LockTypeDef                   Lock;             /*!< CRYP locking object */

  __IO  HAL_CRYP_STATETypeDef       State;            /*!< CRYP peripheral state */

  __IO uint32_t                     ErrorCode;        /*!< CRYP peripheral error code */

  uint32_t                          Version;          /*!< CRYP1 IP version*/

  uint32_t                          KeyIVConfig;      /*!< CRYP peripheral Key and IV configuration flag, used when
                                                           configuration can be skipped */

  uint32_t                          SizesSum;         /*!< Sum of successive payloads lengths (in bytes), stored
                                                           for a single signature computation after several
                                                           messages processing */

#if (USE_HAL_CRYP_REGISTER_CALLBACKS == 1U)
  void (*InCpltCallback)(struct __CRYP_HandleTypeDef *hcryp);      /*!< CRYP Input FIFO transfer completed callback  */
  void (*OutCpltCallback)(struct __CRYP_HandleTypeDef *hcryp);     /*!< CRYP Output FIFO transfer completed callback */
  void (*ErrorCallback)(struct __CRYP_HandleTypeDef *hcryp);       /*!< CRYP Error callback */

  void (* MspInitCallback)(struct __CRYP_HandleTypeDef *hcryp);    /*!< CRYP Msp Init callback  */
  void (* MspDeInitCallback)(struct __CRYP_HandleTypeDef *hcryp);  /*!< CRYP Msp DeInit callback  */

#endif /* (USE_HAL_CRYP_REGISTER_CALLBACKS) */

#if (USE_HAL_CRYP_SUSPEND_RESUME == 1U)

  __IO HAL_SuspendTypeDef     SuspendRequest;          /*!< CRYP peripheral suspension request flag */

  CRYP_ConfigTypeDef          Init_saved;              /*!< copy of CRYP required parameters when processing
                                                            is suspended */

  uint32_t                    *pCrypInBuffPtr_saved;   /*!< copy of CRYP input pointer when processing
                                                            is suspended */

  uint32_t                    *pCrypOutBuffPtr_saved;  /*!< copy of CRYP output pointer when processing
                                                            is suspended */

  uint32_t                    CrypInCount_saved;       /*!< copy of CRYP input data counter when processing
                                                            is suspended */

  uint32_t                    CrypOutCount_saved;      /*!< copy of CRYP output data counter when processing
                                                            is suspended */

  uint32_t                    Phase_saved;             /*!< copy of CRYP authentication phase when processing
                                                            is suspended */

  __IO HAL_CRYP_STATETypeDef  State_saved;             /*!< copy of CRYP peripheral state when processing
                                                            is suspended */

  uint32_t                    IV_saved[4];             /*!< copy of Initialisation Vector registers */

  uint32_t                    SUSPxR_saved[16];        /*!< copy of suspension registers */

  uint32_t                    CR_saved;                /*!< copy of CRYP control register  when processing
                                                            is suspended*/

  uint32_t                    Key_saved[8];            /*!< copy of key registers */

  uint16_t                    Size_saved;              /*!< copy of input buffer size */

  uint16_t                    CrypHeaderCount_saved;   /*!< copy of CRYP header data counter when processing
                                                            is suspended */

  uint32_t                    SizesSum_saved;          /*!< copy of SizesSum when processing is suspended */

  uint32_t                    ResumingFlag;            /*!< resumption flag to bypass steps already carried out */

  uint32_t                    SuspendedProcessing;     /*< Report whether interruption or DMA-mode processing
                                                           was suspended */
#endif /* USE_HAL_CRYP_SUSPEND_RESUME */

} CRYP_HandleTypeDef;


/**
  * @}
  */

#if (USE_HAL_CRYP_REGISTER_CALLBACKS == 1U)
/** @defgroup HAL_CRYP_Callback_ID_enumeration_definition HAL CRYP Callback ID enumeration definition
  * @brief  HAL CRYP Callback ID enumeration definition
  * @{
  */
typedef enum
{
  HAL_CRYP_INPUT_COMPLETE_CB_ID    = 0x01U,    /*!< CRYP Input FIFO transfer completed callback ID  */
  HAL_CRYP_OUTPUT_COMPLETE_CB_ID   = 0x02U,    /*!< CRYP Output FIFO transfer completed callback ID */
  HAL_CRYP_ERROR_CB_ID             = 0x03U,    /*!< CRYP Error callback ID                          */
  HAL_CRYP_MSPINIT_CB_ID           = 0x04U,    /*!< CRYP MspInit callback ID                        */
  HAL_CRYP_MSPDEINIT_CB_ID         = 0x05U     /*!< CRYP MspDeInit callback ID                      */

} HAL_CRYP_CallbackIDTypeDef;
/**
  * @}
  */

/** @defgroup HAL_CRYP_Callback_pointer_definition HAL CRYP Callback pointer definition
  * @brief  HAL CRYP Callback pointer definition
  * @{
  */

typedef  void (*pCRYP_CallbackTypeDef)(CRYP_HandleTypeDef *hcryp);    /*!< pointer to a common CRYP callback function */

/**
  * @}
  */

#endif /* USE_HAL_CRYP_REGISTER_CALLBACKS */

/* Exported constants --------------------------------------------------------*/
/** @defgroup CRYP_Exported_Constants CRYP Exported Constants
  * @{
  */

/** @defgroup CRYP_Error_Definition   CRYP Error Definition
  * @{
  */
#define HAL_CRYP_ERROR_NONE              0x00000000U  /*!< No error        */
#define HAL_CRYP_ERROR_WRITE             0x00000001U  /*!< Write error     */
#define HAL_CRYP_ERROR_READ              0x00000002U  /*!< Read error      */
#define HAL_CRYP_ERROR_DMA               0x00000004U  /*!< DMA error      */
#define HAL_CRYP_ERROR_BUSY              0x00000008U  /*!< Busy flag error */
#define HAL_CRYP_ERROR_TIMEOUT           0x00000010U  /*!< Timeout error   */
#define HAL_CRYP_ERROR_NOT_SUPPORTED     0x00000020U  /*!< Not supported mode */
#define HAL_CRYP_ERROR_AUTH_TAG_SEQUENCE 0x00000040U  /*!< Sequence are not respected only for GCM or CCM */
#if (USE_HAL_CRYP_REGISTER_CALLBACKS == 1U)
#define  HAL_CRYP_ERROR_INVALID_CALLBACK ((uint32_t)0x00000080U)    /*!< Invalid Callback error  */
#endif /* USE_HAL_CRYP_REGISTER_CALLBACKS */
#define HAL_CRYP_ERROR_KEY               0x00000100U  /*!< Key error */
#define HAL_CRYP_ERROR_RNG               0x00000200U  /*!< RNG error */

/**
  * @}
  */


/** @defgroup CRYP_Data_Width_Unit CRYP Data Width Unit
  * @{
  */

#define CRYP_DATAWIDTHUNIT_WORD   0x00000000U  /*!< By default, size unit is word */
#define CRYP_DATAWIDTHUNIT_BYTE   0x00000001U  /*!< By default, size unit is byte */

/**
  * @}
  */

/** @defgroup CRYP_Header_Width_Unit CRYP Header Width Unit
  * @{
  */

#define CRYP_HEADERWIDTHUNIT_WORD   0x00000000U  /*!< By default, size unit is word */
#define CRYP_HEADERWIDTHUNIT_BYTE   0x00000001U  /*!< By default, size unit is byte */

/**
  * @}
  */

#if !defined(USE_HAL_SAES_ONLY) || (USE_HAL_SAES_ONLY == 1)
/** @defgroup SAES_CR_CHMOD SAES CHMOD Selection
  * @{
  */

#define SAES_CR_CHMOD_AES_ECB     ((uint32_t)0x00000000)
#define SAES_CR_CHMOD_AES_CBC     (SAES_CR_CHMOD_0)
#define SAES_CR_CHMOD_AES_CTR     (SAES_CR_CHMOD_1)
#define SAES_CR_CHMOD_AES_GCM     (SAES_CR_CHMOD_0 | SAES_CR_CHMOD_1)
#define SAES_CR_CHMOD_AES_CCM     (SAES_CR_CHMOD_2)

/**
  * @}
  */
#endif /* USE_HAL_SAES_ONLY */

/** @defgroup CRYP_CR_ALGOMODE CRYP Algorithm Mode
  * @{
  */

#define CRYP_AES_ECB              (CRYP_CR_ALGOMODE_2)
#define CRYP_AES_CBC              (CRYP_CR_ALGOMODE_0 | CRYP_CR_ALGOMODE_2)
#define CRYP_AES_CTR              (CRYP_CR_ALGOMODE_1 | CRYP_CR_ALGOMODE_2)
#define CRYP_AES_KEY              (CRYP_CR_ALGOMODE_0 | CRYP_CR_ALGOMODE_1 | CRYP_CR_ALGOMODE_2)
#define CRYP_AES_GCM              (CRYP_CR_ALGOMODE_3)
#define CRYP_AES_CCM              (CRYP_CR_ALGOMODE_0 | CRYP_CR_ALGOMODE_3)

/**
  * @}
  */

/** @defgroup CRYP_Key_Size CRYP Key Size
  * @{
  */

#define CRYP_KEYSIZE_128B         0x00000000U
#define CRYP_KEYSIZE_192B         CRYP_CR_KEYSIZE_0
#define CRYP_KEYSIZE_256B         CRYP_CR_KEYSIZE_1

/**
  * @}
  */

/** @defgroup CRYP_Data_Type CRYP Data Type
  * @{
  */

#define CRYP_DATATYPE_32B         0x00000000U
#define CRYP_DATATYPE_16B         CRYP_CR_DATATYPE_0
#define CRYP_DATATYPE_8B          CRYP_CR_DATATYPE_1
#define CRYP_DATATYPE_1B          CRYP_CR_DATATYPE

#define CRYP_NO_SWAP              CRYP_DATATYPE_32B      /*!< 32-bit data type (no swapping)        */
#define CRYP_HALFWORD_SWAP        CRYP_DATATYPE_16B      /*!< 16-bit data type (half-word swapping) */
#define CRYP_BYTE_SWAP            CRYP_DATATYPE_8B       /*!< 8-bit data type (byte swapping)       */
#define CRYP_BIT_SWAP             CRYP_DATATYPE_1B       /*!< 1-bit data type (bit swapping)        */
/**
  * @}
  */

/** @defgroup CRYP_Interrupt  CRYP and SAES peripherals interrupts
  * @{
  */

#define CRYP_IT_INI       CRYP_IMSCR_INIM   /*!< CRYP peripheral input FIFO Interrupt */
#define CRYP_IT_OUTI      CRYP_IMSCR_OUTIM  /*!< CRYP peripheral output FIFO Interrupt */

#define CRYP_IT_CCFIE     SAES_IER_CCFIE    /*!< SAES peripheral computation Complete interrupt enable */
#define CRYP_IT_RWEIE     SAES_IER_RWEIE    /*!< SAES peripheral read or write Error interrupt enable  */
#define CRYP_IT_KEIE      SAES_IER_KEIE     /*!< SAES peripheral key error interrupt enable  */
#define CRYP_IT_RNGEIE    SAES_IER_RNGEIE   /*!< SAES peripheral RNG error interrupt enable  */

/**
  * @}
  */

/** @defgroup CRYP_Flags CRYP & SAES Flags
  * @{
  */

/* Flags in the SR register */
#define CRYP_FLAG_IFEM     CRYP_SR_IFEM      /*!< CRYP peripheral Input FIFO is empty */
#define CRYP_FLAG_IFNF     CRYP_SR_IFNF      /*!< CRYP peripheral Input FIFO is not Full */
#define CRYP_FLAG_OFNE     CRYP_SR_OFNE      /*!< CRYP peripheral Output FIFO is not empty */
#define CRYP_FLAG_OFFU     CRYP_SR_OFFU      /*!< CRYP peripheral Output FIFO is Full */
#define CRYP_FLAG_KERF     CRYP_SR_KERF      /*!< CRYP peripheral Key error flag */
/* Flags in the RISR register */
#define CRYP_FLAG_OUTRIS   0x01000002U  /*!< Output FIFO service raw interrupt status */
#define CRYP_FLAG_INRIS    0x01000001U  /*!< Input FIFO service raw interrupt status*/

#define CRYP_FLAG_BUSY     CRYP_SR_BUSY      /*!< The CRYP peripheral is currently processing a block of data
                                                 or a key preparation (for AES decryption). */


#define CRYP_FLAG_KEYVALID CRYP_SR_KEYVALID  /*!< CRYP or SAES peripheral Key valid flag */

#define SAES_FLAG_BUSY     SAES_SR_BUSY      /*!< The SAES peripheral is currently processing a block of data
                                                  or a key preparation (for AES decryption). */

#define CRYP_FLAG_WRERR    (SAES_SR_WRERR | 0x80000000U) /*!< SAES peripheral Write Error flag */
#define CRYP_FLAG_RDERR    (SAES_SR_RDERR | 0x80000000U) /*!< SAES peripheral Read error flag */
#define CRYP_FLAG_CCF      SAES_SR_CCF                   /*!< SAES peripheral Computation completed flag 
                                                              as AES_ISR_CCF */
#define CRYP_FLAG_KEIF     SAES_ISR_KEIF                 /*!< SAES peripheral Key error interrupt flag */
#define CRYP_FLAG_RWEIF    SAES_ISR_RWEIF                /*!< SAES peripheral Read or Write error Interrupt flag */
#define CRYP_FLAG_RNGEIF   SAES_ISR_RNGEIF               /*!< SAES peripheral RNG error Interrupt flag */
/**
  * @}
  */

/** @defgroup CRYP_CLEAR_Flags SAES peripheral Clear Flags
  * @{
  */
#define CRYP_CLEAR_CCF      SAES_ICR_CCF    /*!< SAES peripheral clear Computation Complete Flag */
#define CRYP_CLEAR_RWEIF    SAES_ICR_RWEIF  /*!< SAES peripheral clear Error Flag : RWEIF in SAES_ISR and
                                              both RDERR and WRERR flags in SAES_SR */
#define CRYP_CLEAR_KEIF     SAES_ICR_KEIF   /*!< SAES peripheral clear Key Error Flag: KEIF in SAES_ISR */
#define CRYP_CLEAR_RNGEIF   SAES_ICR_RNGEIF /*!< SAES peripheral clear RNG error Flag  */

/**
  * @}
  */


/** @defgroup CRYP_Configuration_Skip CRYP Key and IV Configuration Skip Mode
  * @{
  */

#define CRYP_KEYIVCONFIG_ALWAYS        0x00000000U            /*!< Peripheral Key and IV configuration 
                                                                   to do systematically */
#define CRYP_KEYIVCONFIG_ONCE          0x00000001U            /*!< Peripheral Key and IV configuration
                                                                   to do only once */
#define CRYP_KEYNOCONFIG               0x00000002U            /*!< Peripheral Key configuration to not do */

/**
  * @}
  */

/** @defgroup CRYP_Key_Mode CRYP or SAES Key Mode
  * @{
  */

#define CRYP_KEYMODE_NORMAL         0x00000000U         /*!< Normal key usage, Key registers are freely usable */
#define CRYP_KEYMODE_WRAPPED        SAES_CR_KMOD_0      /*!< Only for SAES, Wrapped key: to encrypt 
                                                             or decrypt AES keys */
#define CRYP_KEYMODE_SHARED         SAES_CR_KMOD_1      /*!< Key shared by SAES peripheral */
/**
  * @}
  */

/** @defgroup CRYP_Key_Select SAES Key Select
  * @{
  */

#define CRYP_KEYSEL_NORMAL       0x00000000U            /*!< Normal key, key registers SAES_KEYx or CRYP_KEYx */
#define CRYP_KEYSEL_HW           SAES_CR_KEYSEL_0       /*!< Only for SAES, Hardware key : derived hardware 
                                                             unique key (DHUK 256-bit) */
#define CRYP_KEYSEL_SW           SAES_CR_KEYSEL_1       /*!< Only for SAES, Software key : boot hardware 
                                                             key BHK (256-bit) */
#define CRYP_KEYSEL_HSW          SAES_CR_KEYSEL_2       /*!< Only for SAES, DHUK XOR BHK Hardware unique 
                                                             key XOR software key */
#define CRYP_KEYSEL_AHK          (SAES_CR_KEYSEL_1|SAES_CR_KEYSEL_0)  /*!< Only for SAES, Software key : 
                                                                      application hardware key AHK (128- or 256-bit) */
#define CRYP_KEYSEL_DUK_AHK      (SAES_CR_KEYSEL_2|SAES_CR_KEYSEL_0)   /*!< Only for SAES, DHUK XOR AHK */
#define CRYP_KEYSEL_TEST_KEY     (SAES_CR_KEYSEL_2|SAES_CR_KEYSEL_1|SAES_CR_KEYSEL_0) /*!< Test mode key (256-bit 
                                                                               hardware constant key 0xA5A5...A5A5) */
/**
  * @}
  */

#if !defined(USE_HAL_SAES_ONLY) || (USE_HAL_SAES_ONLY == 1)
/** @defgroup CRYP_Key_Protection SAES Key Protection
  * @{
  */

#define CRYP_KEYPROT_ENABLE         SAES_CR_KEYPROT     /*!< Only for SAES, Key can't be shared between 
                                                             two applications with different security contexts */
#define CRYP_KEYPROT_DISABLE        0x00000000U         /*!< Only for SAES, Key can be shared between 
                                                             two applications with different security contexts */

/**
  * @}
  */

/** @defgroup CRYP_Key_Shared SAES Key Shared with Peripheral
  * @{
  */

#define CRYP_KEYSHARED_CRYP         0x00000000U     /*!< Only for SAES, key is shared with CRYP peripheral */

/**
  * @}
  */

/** @defgroup CRYP_Mode SAES processing mode
  * @{
  */

#define CRYP_MODE_ENCRYPT                0x00000000U             /*!< SAES peripheral encryption mode   */
#define CRYP_MODE_KEY_DERIVATION         SAES_CR_MODE_0          /*!< SAES peripheral key derivation    */
#define CRYP_MODE_DECRYPT                SAES_CR_MODE_1          /*!< SAES peripheral decryption mode   */

/**
  * @}
  */
#endif /* USE_HAL_SAES_ONLY */

/** @defgroup CRYP_Mode SAES processing mode
  * @{
  */

#define CRYP_OPERATINGMODE_ENCRYPT       0x00000000U             /*!< CRYP peripheral encryption mode   */
#define CRYP_OPERATINGMODE_DECRYPT       CRYP_CR_ALGODIR         /*!< CRYP peripheral decryption mode   */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup CRYP_Exported_Macros CRYP Exported Macros
  * @{
  */

/**
  * @brief  Inform about which IP is the current INSTANCE: CRYP or SAES.
  * @param  INSTANCE: specifies the HW instance.
  * @retval None
  */
#define IS_CRYP_INSTANCE(INSTANCE) ((INSTANCE) == CRYP)
#define IS_SAES_INSTANCE(INSTANCE) ((INSTANCE) == SAES)

/** @brief Reset CRYP handle state
  * @param  __HANDLE__ specifies the CRYP handle.
  * @retval None
  */
#if (USE_HAL_CRYP_REGISTER_CALLBACKS == 1U)
#define __HAL_CRYP_RESET_HANDLE_STATE(__HANDLE__) do{\
                                                      (__HANDLE__)->State = HAL_CRYP_STATE_RESET;\
                                                      (__HANDLE__)->MspInitCallback = NULL;\
                                                      (__HANDLE__)->MspDeInitCallback = NULL;\
                                                     }while(0)
#else
#define __HAL_CRYP_RESET_HANDLE_STATE(__HANDLE__) ( (__HANDLE__)->State = HAL_CRYP_STATE_RESET)
#endif /* USE_HAL_CRYP_REGISTER_CALLBACKS */

/**
  * @brief  Enable/Disable the CRYP or SAES peripheral.
  * @param  __HANDLE__: specifies the CRYP handle.
  * @retval None
  */

#define __HAL_CRYP_ENABLE(__HANDLE__) ((IS_CRYP_INSTANCE((__HANDLE__)->Instance)) ? \
                                       (((CRYP_TypeDef *)((__HANDLE__)->Instance))->CR |=  CRYP_CR_CRYPEN) :\
                                       (((SAES_TypeDef *)((__HANDLE__)->Instance))->CR |=  SAES_CR_EN))

#define __HAL_CRYP_DISABLE(__HANDLE__) ((IS_CRYP_INSTANCE((__HANDLE__)->Instance)) ? \
                                        (((CRYP_TypeDef *)((__HANDLE__)->Instance))->CR &=  ~CRYP_CR_CRYPEN) :\
                                        (((SAES_TypeDef *)((__HANDLE__)->Instance))->CR &=  ~SAES_CR_EN))

/** @brief  Check whether the specified CRYP or SAES peripheral status flag is set or not.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg CRYP_FLAG_BUSY: The CRYP core is currently processing a block of data
  *                                 or a key preparation (for AES decryption)
  *            @arg @ref CRYP_FLAG_KEYVALID Key valid flag
  *            @arg @ref CRYP_FLAG_KEIF Key error flag
  *            @arg CRYP_FLAG_IFEM: Input FIFO is empty                           (CRYP peripheral only)
  *            @arg CRYP_FLAG_IFNF: Input FIFO is not full                        (CRYP peripheral only)
  *            @arg CRYP_FLAG_INRIS: Input FIFO service raw interrupt is pending  (CRYP peripheral only)
  *            @arg CRYP_FLAG_OFNE: Output FIFO is not empty                      (CRYP peripheral only)
  *            @arg CRYP_FLAG_OFFU: Output FIFO is full                           (CRYP peripheral only)
  *            @arg CRYP_FLAG_OUTRIS: Input FIFO service raw interrupt is pending (CRYP peripheral only)
  *            @arg @ref CRYP_FLAG_WRERR Write Error flag                         (SAES peripheral only)
  *            @arg @ref CRYP_FLAG_RDERR Read Error flag                          (SAES peripheral only)
  *            @arg @ref CRYP_FLAG_CCF Computation Complete flag                  (SAES peripheral only)
  *            @arg @ref CRYP_FLAG_RWEIF Read/write Error flag                    (SAES peripheral only)
  *            @arg @ref CRYP_FLAG_RNGEIF RNG Error flag                          (SAES peripheral only)
  * @retval The state of __FLAG__ (TRUE or FALSE).
  */
#define CRYP_FLAG_MASK  0x0000001FU

#define __HAL_CRYP_GET_FLAG(__HANDLE__, __FLAG__) ((IS_CRYP_INSTANCE((__HANDLE__)->Instance)) ?\
                                                   ((__FLAG__) == CRYP_FLAG_KEYVALID )?((((((CRYP_TypeDef *) \
                                                       ((__HANDLE__)->Instance))))->SR \
                                                       & (CRYP_FLAG_KEYVALID)) == (CRYP_FLAG_KEYVALID)) : \
                                                   ((__FLAG__) == CRYP_FLAG_BUSY )?((((((CRYP_TypeDef *) \
                                                       ((__HANDLE__)->Instance))))->SR \
                                                       & (CRYP_FLAG_BUSY)) == (CRYP_FLAG_BUSY)) : \
                                                   ((__FLAG__) == CRYP_FLAG_KEIF )?((((((CRYP_TypeDef *) \
                                                       ((__HANDLE__)->Instance))))->SR \
                                                       & (CRYP_FLAG_KERF)) == (CRYP_FLAG_KERF)) : \
                                                   ((((uint8_t)((__FLAG__) >> 24)) == 0x01U)?(((((CRYP_TypeDef *) \
                                                       ((__HANDLE__)->Instance))->RISR) & ((__FLAG__) & \
                                                           CRYP_FLAG_MASK)) == ((__FLAG__) & CRYP_FLAG_MASK)): \
                                                    (((((CRYP_TypeDef *)((__HANDLE__)->Instance))->RISR) & ((__FLAG__)\
                                                        & CRYP_FLAG_MASK)) == ((__FLAG__) & CRYP_FLAG_MASK))) :\
                                                   (\
                                                    ((__FLAG__) == CRYP_FLAG_KEYVALID )?((((SAES_TypeDef *)\
                                                        (((SAES_TypeDef *)((__HANDLE__)->Instance))))->SR \
                                                        & (CRYP_FLAG_KEYVALID)) == (CRYP_FLAG_KEYVALID)) : \
                                                    ((__FLAG__) == CRYP_FLAG_BUSY )?((((SAES_TypeDef *) \
                                                        ((__HANDLE__)->Instance))->SR \
                                                        & (CRYP_FLAG_BUSY)) == (CRYP_FLAG_BUSY)) : \
                                                    ((__FLAG__) == CRYP_FLAG_WRERR )?((((SAES_TypeDef *) \
                                                        ((__HANDLE__)->Instance))->SR \
                                                        & (CRYP_FLAG_WRERR & 0x7FFFFFFFU)) == \
                                                        (CRYP_FLAG_WRERR & 0x7FFFFFFFU)) : \
                                                    ((__FLAG__) == CRYP_FLAG_RDERR )?((((SAES_TypeDef *) \
                                                        ((__HANDLE__)->Instance))->SR \
                                                        & (CRYP_FLAG_RDERR & 0x7FFFFFFFU)) == \
                                                        (CRYP_FLAG_RDERR & 0x7FFFFFFFU)) : \
                                                    ((__FLAG__) == CRYP_FLAG_RNGEIF )?((((SAES_TypeDef *) \
                                                        ((__HANDLE__)->Instance))->ISR \
                                                        & (CRYP_FLAG_RNGEIF)) == (CRYP_FLAG_RNGEIF)) : \
                                                    ((__FLAG__) == CRYP_FLAG_KEIF )?((((SAES_TypeDef *) \
                                                        ((__HANDLE__)->Instance))->ISR \
                                                        & (CRYP_FLAG_KEIF)) == (CRYP_FLAG_KEIF)) : \
                                                    ((__FLAG__) == CRYP_FLAG_RWEIF )?((((SAES_TypeDef *) \
                                                        ((__HANDLE__)->Instance))->ISR \
                                                        & (CRYP_FLAG_RWEIF)) == (CRYP_FLAG_RWEIF)) : \
                                                    ((((SAES_TypeDef *)((__HANDLE__)->Instance))->ISR & \
                                                      (CRYP_FLAG_CCF)) == (CRYP_FLAG_CCF))))

#if !defined(USE_HAL_SAES_ONLY) || (USE_HAL_SAES_ONLY == 1)
/** @brief  Clear the SAES peripheral pending status flag.
  * @param  __HANDLE__ specifies the SAES handle.
  * @param  __FLAG__ specifies the flag to clear.
  *         This parameter can be one of the following values:
  *            @arg @ref CRYP_CLEAR_RWEIF Read (RDERR), Write (WRERR) or Read/write (RWEIF) Error  Flag Clear
  *            @arg @ref CRYP_CLEAR_CCF Computation Complete Flag (CCF) Clear
  *            @arg @ref CRYP_CLEAR_KEIF Key error interrupt flag clear
  *            @arg @ref CRYP_CLEAR_RNGEIF RNG error interrupt flag clear
  * @retval None
  */
#define __HAL_CRYP_CLEAR_FLAG(__HANDLE__, __FLAG__) WRITE_REG(((SAES_TypeDef *)((__HANDLE__)->Instance))->ICR,\
                                                              (__FLAG__))
#endif /* USE_HAL_SAES_ONLY */

#if !defined(USE_HAL_CRYP_ONLY) || (USE_HAL_CRYP_ONLY == 1)
/** @brief  Check whether the specified CRYP peripheral interrupt is set or not.
  * @param  __HANDLE__: specifies the CRYP handle.
  * @param  __INTERRUPT__: specifies the interrupt to check.
  *         This parameter can be one of the following values:
  *            @arg CRYP_IT_INI: Input FIFO service masked interrupt status
  *            @arg CRYP_IT_OUTI: Output FIFO service masked interrupt status
  * @retval The state of __INTERRUPT__ (TRUE or FALSE).
  */

#define __HAL_CRYP_GET_IT(__HANDLE__, __INTERRUPT__) ((((CRYP_TypeDef *)((__HANDLE__)->Instance))->MISR\
                                                       & (__INTERRUPT__)) == (__INTERRUPT__))
#endif /* USE_HAL_CRYP_ONLY */

/**
  * @brief  Enable the CRYP or SAES peripheral interrupt.
  * @param  __HANDLE__: specifies the CRYP handle.
  * @param  __INTERRUPT__:  Interrupt.
  *         This parameter can be one of the following values:
  *            @arg @ref  CRYP_IT_INI    Input FIFO service interrupt mask          (CRYP peripheral only)
  *            @arg @ref  CRYP_IT_OUTI   Output FIFO service interrupt mask         (CRYP peripheral only)
  *            @arg @ref  CRYP_IT_RWEIE  Error interrupt (used for RDERR and WRERR) (SAES peripheral only)
  *            @arg @ref  CRYP_IT_CCFIE  Computation Complete interrupt             (SAES peripheral only)
  *            @arg @ref  CRYP_IT_KEIE   Key error interrupt                        (SAES peripheral only)
  *            @arg @ref  CRYP_IT_RNGEIE RNG interrupt                              (SAES peripheral only)
  * @retval None
  */

#define __HAL_CRYP_ENABLE_IT(__HANDLE__, __INTERRUPT__)  ((IS_CRYP_INSTANCE((__HANDLE__)->Instance)) ?\
                                                          ((((CRYP_TypeDef *)((__HANDLE__)->Instance))->IMSCR) |= \
                                                              (__INTERRUPT__)) : ((((SAES_TypeDef *) \
                                                                  ((__HANDLE__)->Instance))->IER) |= (__INTERRUPT__)))

/**
  * @brief  Disable the CRYP or SAES peripheral interrupt.
  * @param  __HANDLE__: specifies the CRYP handle.
  * @param  __INTERRUPT__:  Interrupt.
  *         This parameter can be one of the following values:
  *            @arg @ref  CRYP_IT_INI    Input FIFO service interrupt mask          (CRYP peripheral only)
  *            @arg @ref  CRYP_IT_OUTI   Output FIFO service interrupt mask         (CRYP peripheral only)
  *            @arg @ref  CRYP_IT_RWEIE  Error interrupt (used for RDERR and WRERR) (SAES peripheral only)
  *            @arg @ref  CRYP_IT_CCFIE  Computation Complete interrupt             (SAES peripheral only)
  *            @arg @ref  CRYP_IT_KEIE   Key error interrupt                        (SAES peripheral only)
  *            @arg @ref  CRYP_IT_RNGEIE RNG interrupt                              (SAES peripheral only)
  * @retval None
  */

#define __HAL_CRYP_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((IS_CRYP_INSTANCE((__HANDLE__)->Instance)) ?\
                                                          ((((CRYP_TypeDef *)((__HANDLE__)->Instance))->IMSCR) &= \
                                                              ~(__INTERRUPT__)) : ((((SAES_TypeDef *) \
                                                                  ((__HANDLE__)->Instance))->IER) &= ~(__INTERRUPT__)))

#if !defined(USE_HAL_SAES_ONLY) || (USE_HAL_SAES_ONLY == 1)
/** @brief  Check whether the specified SAES peripheral interrupt source is enabled or not.
  * @param  __HANDLE__ specifies the CRYP handle.
  * @param __INTERRUPT__  interrupt source to check
  *         This parameter can be one of the following values for TinyAES:
  *            @arg @ref CRYP_IT_RWEIE Error interrupt (used for RDERR and WRERR)
  *            @arg @ref CRYP_IT_CCFIE Computation Complete interrupt
  *            @arg @ref CRYP_IT_KEIE Key error interrupt
  *            @arg @ref CRYP_IT_RNGEIE RNG error interrupt
  * @retval State of interruption (TRUE or FALSE).
  */

#define __HAL_CRYP_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((SAES_TypeDef *)((__HANDLE__)->Instance))->IER\
                                                              & (__INTERRUPT__)) == (__INTERRUPT__))
#endif /* USE_HAL_SAES_ONLY */
/**
  * @}
  */

/* Include CRYP HAL Extended module */
#include "stm32n6xx_hal_cryp_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @defgroup CRYP_Exported_Functions CRYP Exported Functions
  * @{
  */

/** @addtogroup CRYP_Exported_Functions_Group1
  * @{
  */
HAL_StatusTypeDef HAL_CRYP_Init(CRYP_HandleTypeDef *hcryp);
HAL_StatusTypeDef HAL_CRYP_DeInit(CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_MspInit(CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef *hcryp);
HAL_StatusTypeDef HAL_CRYP_SetConfig(CRYP_HandleTypeDef *hcryp, const CRYP_ConfigTypeDef *pConf);
HAL_StatusTypeDef HAL_CRYP_GetConfig(CRYP_HandleTypeDef *hcryp, CRYP_ConfigTypeDef *pConf);
#if (USE_HAL_CRYP_REGISTER_CALLBACKS == 1U)
HAL_StatusTypeDef HAL_CRYP_RegisterCallback(CRYP_HandleTypeDef *hcryp, HAL_CRYP_CallbackIDTypeDef CallbackID,
                                            pCRYP_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_CRYP_UnRegisterCallback(CRYP_HandleTypeDef *hcryp, HAL_CRYP_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_CRYP_REGISTER_CALLBACKS */
#if (USE_HAL_CRYP_SUSPEND_RESUME == 1U)
void HAL_CRYP_ProcessSuspend(CRYP_HandleTypeDef *hcryp);
HAL_StatusTypeDef HAL_CRYP_DMAProcessSuspend(CRYP_HandleTypeDef *hcryp);
HAL_StatusTypeDef HAL_CRYP_Suspend(CRYP_HandleTypeDef *hcryp);
HAL_StatusTypeDef HAL_CRYP_Resume(CRYP_HandleTypeDef *hcryp);
#endif /* defined (USE_HAL_CRYP_SUSPEND_RESUME) */
/**
  * @}
  */

/** @addtogroup CRYP_Exported_Functions_Group2
  * @{
  */

/* encryption/decryption ***********************************/
HAL_StatusTypeDef HAL_CRYP_Encrypt(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint16_t Size, uint32_t *Output,
                                   uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_Decrypt(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint16_t Size, uint32_t *Output,
                                   uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint16_t Size, uint32_t *Output);
HAL_StatusTypeDef HAL_CRYP_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint16_t Size, uint32_t *Output);
HAL_StatusTypeDef HAL_CRYP_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint16_t Size, uint32_t *Output);
HAL_StatusTypeDef HAL_CRYP_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint16_t Size, uint32_t *Output);

/**
  * @}
  */


/** @addtogroup CRYP_Exported_Functions_Group3
  * @{
  */
/* Interrupt Handler functions  **********************************************/
void HAL_CRYP_IRQHandler(CRYP_HandleTypeDef *hcryp);
HAL_CRYP_STATETypeDef HAL_CRYP_GetState(const CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_InCpltCallback(CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_OutCpltCallback(CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_ErrorCallback(CRYP_HandleTypeDef *hcryp);
uint32_t HAL_CRYP_GetError(const CRYP_HandleTypeDef *hcryp);

/**
  * @}
  */

/**
  * @}
  */

/* Private macros --------------------------------------------------------*/
/** @defgroup CRYP_Private_Macros   CRYP Private Macros
  * @{
  */

/** @defgroup CRYP_IS_CRYP_Definitions CRYP Private macros to check input parameters
  * @{
  */

#define IS_CRYP_ALGORITHM(ALGORITHM) (((ALGORITHM)  == CRYP_AES_ECB)   || \
                                      ((ALGORITHM)  == CRYP_AES_CBC)   || \
                                      ((ALGORITHM)  == CRYP_AES_CTR)   || \
                                      ((ALGORITHM)  == CRYP_AES_GCM)   || \
                                      ((ALGORITHM)  == CRYP_AES_CCM))

#define IS_CRYP_KEYSIZE(KEYSIZE) (((KEYSIZE) == CRYP_KEYSIZE_128B)   || \
                                  ((KEYSIZE) == CRYP_KEYSIZE_192B)   || \
                                  ((KEYSIZE) == CRYP_KEYSIZE_256B))

#define IS_CRYP_DATATYPE(DATATYPE) (((DATATYPE) == CRYP_DATATYPE_32B) || \
                                    ((DATATYPE) == CRYP_DATATYPE_16B) || \
                                    ((DATATYPE) == CRYP_DATATYPE_8B)  || \
                                    ((DATATYPE) == CRYP_DATATYPE_1B))

#define IS_CRYP_INIT(CONFIG) (((CONFIG) == CRYP_KEYIVCONFIG_ALWAYS) || \
                              ((CONFIG) == CRYP_KEYIVCONFIG_ONCE)   || \
                              ((CONFIG) == CRYP_KEYNOCONFIG))

#define IS_CRYP_KEYIVCONFIG(CONFIG) (((KEYSIZE) == CRYP_KEYSIZE_128B) || \
                                     ((KEYSIZE) == CRYP_KEYSIZE_192B)  || \
                                     ((KEYSIZE) == CRYP_KEYSIZE_256B))

#if !defined(USE_HAL_SAES_ONLY) || (USE_HAL_SAES_ONLY == 1)
#define IS_CRYP_KEYMODE(MODE) (((MODE) == CRYP_KEYMODE_NORMAL) || \
                               ((MODE) == CRYP_KEYMODE_SHARED))

#define IS_SAES_ALGORITHM(ALGORITHM) (((ALGORITHM) == CRYP_AES_ECB)   || \
                                      ((ALGORITHM) == CRYP_AES_CBC)   || \
                                      ((ALGORITHM) == CRYP_AES_CTR)   || \
                                      ((ALGORITHM) == CRYP_AES_GCM)   || \
                                      ((ALGORITHM) == CRYP_AES_CCM))

#define IS_SAES_KEYSIZE(KEYSIZE) (((KEYSIZE) == CRYP_KEYSIZE_128B)  || \
                                  ((KEYSIZE) == CRYP_KEYSIZE_256B))

#define IS_SAES_DATATYPE(DATATYPE) (((DATATYPE) == SAES_DATATYPE_32B) || \
                                    ((DATATYPE) == SAES_DATATYPE_16B) || \
                                    ((DATATYPE) == SAES_DATATYPE_8B)  || \
                                    ((DATATYPE) == SAES_DATATYPE_1B))

#define IS_SAES_KEYMODE(MODE) (((MODE) == CRYP_KEYMODE_NORMAL)  || \
                               ((MODE) == CRYP_KEYMODE_WRAPPED) || \
                               ((MODE) == CRYP_KEYMODE_SHARED))

#define IS_SAES_KEYPROT(PROTECTION) (((PROTECTION) == CRYP_KEYPROT_ENABLE)  || \
                                     ((PROTECTION) == CRYP_KEYPROT_DISABLE))

#define IS_SAES_KEYSEL(SELECTION) (((SELECTION) == CRYP_KEYSEL_NORMAL)   || \
                                   ((SELECTION) == CRYP_KEYSEL_HW)       || \
                                   ((SELECTION) == CRYP_KEYSEL_SW)       || \
                                   ((SELECTION) == CRYP_KEYSEL_HSW)      || \
                                   ((SELECTION) == CRYP_KEYSEL_AHK)      || \
                                   ((SELECTION) == CRYP_KEYSEL_DUK_AHK)  || \
                                   ((SELECTION) == CRYP_KEYSEL_TEST_KEY))

#define IS_SAES_KEYSHARED(PERIPHERAL) ((PERIPHERAL) == CRYP_KEYSHARED_CRYP)
/**
  * @}
  */
#endif /* USE_HAL_SAES_ONLY */

/** @defgroup SAES_CONV_Definitions SAES Private macros to convert input parameters from CRYP peripheral to
     SAES peripheral format
  * @{
  */
#if !defined(USE_HAL_SAES_ONLY) || (USE_HAL_SAES_ONLY == 1)
#define SAES_CONV_DATATYPE(__DATATYPE__) ((__DATATYPE__) >> (CRYP_CR_DATATYPE_Pos - SAES_CR_DATATYPE_Pos))

#define SAES_CONV_KEYSIZE(__KEY__) (((__KEY__)\
                                     & CRYP_CR_KEYSIZE_1) << (SAES_CR_KEYSIZE_Pos - (CRYP_CR_KEYSIZE_Pos + 1U)))

#define SAES_CONV_ALGO(__ALGO__) (((__ALGO__)\
                                   & (CRYP_CR_ALGOMODE_1 | CRYP_CR_ALGOMODE_0)) << (SAES_CR_CHMOD_Pos - \
                                       CRYP_CR_ALGOMODE_Pos))
#endif /* USE_HAL_SAES_ONLY */
#define CRYP_CONV_ALGODIR(__ALGODIR__) (((__ALGODIR__)\
                                         & SAES_CR_MODE_1) >> ((SAES_CR_MODE_Pos + 1U) - CRYP_CR_ALGODIR_Pos))

/**
  * @}
  */


/**
  * @}
  */


/* Private constants ---------------------------------------------------------*/
/** @defgroup CRYP_Private_Constants CRYP Private Constants
  * @{
  */

/**
  * @}
  */
/* Private defines -----------------------------------------------------------*/
/** @defgroup CRYP_Private_Defines CRYP Private Defines
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup CRYP_Private_Variables CRYP Private Variables
  * @{
  */

/**
  * @}
  */
/* Private functions prototypes ----------------------------------------------*/
/** @defgroup CRYP_Private_Functions_Prototypes CRYP Private Functions Prototypes
  * @{
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup CRYP_Private_Functions CRYP Private Functions
  * @{
  */

/**
  * @}
  */


/**
  * @}
  */


#endif /* CRYP */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

#endif /* STM32N6xx_HAL_CRYP_H */
