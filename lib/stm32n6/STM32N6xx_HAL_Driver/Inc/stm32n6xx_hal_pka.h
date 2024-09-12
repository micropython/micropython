/**
  ******************************************************************************
  * @file    stm32n6xx_hal_pka.h
  * @author  MCD Application Team
  * @brief   Header file of PKA HAL module.
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
#ifndef STM32N6xx_HAL_PKA_H
#define STM32N6xx_HAL_PKA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

#if defined(PKA) && defined(HAL_PKA_MODULE_ENABLED)

/** @addtogroup PKA
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup PKA_Exported_Types PKA Exported Types
  * @{
  */

/** @defgroup HAL_state_structure_definition HAL state structure definition
  * @brief  HAL State structures definition
  * @{
  */
typedef enum
{
  HAL_PKA_STATE_RESET   = 0x00U,  /*!< PKA not yet initialized or disabled  */
  HAL_PKA_STATE_READY   = 0x01U,  /*!< PKA initialized and ready for use    */
  HAL_PKA_STATE_BUSY    = 0x02U,  /*!< PKA internal processing is ongoing   */
  HAL_PKA_STATE_ERROR   = 0x03U,  /*!< PKA error state                      */
}
HAL_PKA_StateTypeDef;

/**
  * @}
  */

#if (USE_HAL_PKA_REGISTER_CALLBACKS == 1)
/** @defgroup HAL_callback_id HAL callback ID enumeration
  * @{
  */
typedef enum
{
  HAL_PKA_OPERATION_COMPLETE_CB_ID      = 0x00U,    /*!< PKA End of operation callback ID  */
  HAL_PKA_ERROR_CB_ID                   = 0x01U,    /*!< PKA Error callback ID             */
  HAL_PKA_MSPINIT_CB_ID                 = 0x02U,    /*!< PKA Msp Init callback ID          */
  HAL_PKA_MSPDEINIT_CB_ID               = 0x03U     /*!< PKA Msp DeInit callback ID        */
} HAL_PKA_CallbackIDTypeDef;

/**
  * @}
  */

#endif /* USE_HAL_PKA_REGISTER_CALLBACKS */

/** @defgroup PKA_Error_Code_definition PKA Error Code definition
  * @brief  PKA Error Code definition
  * @{
  */
#define HAL_PKA_ERROR_NONE      (0x00000000U)
#define HAL_PKA_ERROR_ADDRERR   (0x00000001U)
#define HAL_PKA_ERROR_RAMERR    (0x00000002U)
#define HAL_PKA_ERROR_TIMEOUT   (0x00000004U)
#define HAL_PKA_ERROR_OPERATION (0x00000008U)
#if (USE_HAL_PKA_REGISTER_CALLBACKS == 1)
#define HAL_PKA_ERROR_INVALID_CALLBACK  (0x00000010U)    /*!< Invalid Callback error */
#endif /* USE_HAL_PKA_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup PKA_handle_Structure_definition PKA handle Structure definition
  * @brief  PKA handle Structure definition
  * @{
  */
#if (USE_HAL_PKA_REGISTER_CALLBACKS == 1)
typedef struct __PKA_HandleTypeDef
#else
typedef struct
#endif  /* USE_HAL_PKA_REGISTER_CALLBACKS */
{
  PKA_TypeDef                   *Instance;              /*!< Register base address */
  __IO HAL_PKA_StateTypeDef     State;                  /*!< PKA state */
  __IO uint32_t                 ErrorCode;              /*!< PKA Error code */
#if (USE_HAL_PKA_REGISTER_CALLBACKS == 1)
  void (* OperationCpltCallback)(struct __PKA_HandleTypeDef *hpka); /*!< PKA End of operation callback */
  void (* ErrorCallback)(struct __PKA_HandleTypeDef *hpka);         /*!< PKA Error callback            */
  void (* MspInitCallback)(struct __PKA_HandleTypeDef *hpka);       /*!< PKA Msp Init callback         */
  void (* MspDeInitCallback)(struct __PKA_HandleTypeDef *hpka);     /*!< PKA Msp DeInit callback       */
#endif  /* USE_HAL_PKA_REGISTER_CALLBACKS */
} PKA_HandleTypeDef;
/**
  * @}
  */

#if (USE_HAL_PKA_REGISTER_CALLBACKS == 1)
/** @defgroup PKA_Callback_definition PKA Callback pointer definition
  * @brief  PKA Callback pointer definition
  * @{
  */
typedef  void (*pPKA_CallbackTypeDef)(PKA_HandleTypeDef *hpka); /*!< Pointer to a PKA callback function */
/**
  * @}
  */
#endif /* USE_HAL_PKA_REGISTER_CALLBACKS */
/** @defgroup PKA_Operation PKA operation structure definition
  * @brief  Input and output data definition
  * @{
  */

typedef struct
{
  uint32_t scalarMulSize;              /*!< Number of element in scalarMul array */
  uint32_t modulusSize;                /*!< Number of element in modulus, coefA, pointX and pointY arrays */
  uint32_t coefSign;                   /*!< Curve coefficient a sign */
  const uint8_t *coefA;                /*!< Pointer to curve coefficient |a| (Array of modulusSize elements) */
  const uint8_t *coefB;                /*!< pointer to curve coefficient b */
  const uint8_t *modulus;              /*!< Pointer to curve modulus value p (Array of modulusSize elements) */
  const uint8_t *pointX;               /*!< Pointer to point P coordinate xP (Array of modulusSize elements) */
  const uint8_t *pointY;               /*!< Pointer to point P coordinate yP (Array of modulusSize elements) */
  const uint8_t *scalarMul;            /*!< Pointer to scalar multiplier k   (Array of scalarMulSize elements) */
  const uint8_t *primeOrder;           /*!< pointer to order of the curve */
} PKA_ECCMulInTypeDef;

typedef struct
{
  uint32_t primeOrderSize;             /*!< Number of element in primeOrder array */
  uint32_t scalarMulSize;              /*!< Number of element in scalarMul array */
  uint32_t modulusSize;                /*!< Number of element in modulus, coefA, pointX and pointY arrays */
  uint32_t coefSign;                   /*!< Curve coefficient a sign */
  const uint8_t *coefA;                /*!< Pointer to curve coefficient |a| (Array of modulusSize elements) */
  const uint8_t *coefB;                /*!< pointer to curve coefficient b */
  const uint8_t *modulus;              /*!< Pointer to curve modulus value p (Array of modulusSize elements) */
  const uint8_t *pointX;               /*!< Pointer to point P coordinate xP (Array of modulusSize elements) */
  const uint8_t *pointY;               /*!< Pointer to point P coordinate yP (Array of modulusSize elements) */
  const uint8_t *scalarMul;            /*!< Pointer to scalar multiplier k   (Array of scalarMulSize elements) */
  const uint8_t *primeOrder;           /*!< pointer to order of the curve */
} PKA_ECCMulExInTypeDef;

typedef struct
{
  uint32_t modulusSize;                /*!< Number of element in coefA, coefB, modulus, pointX and pointY arrays */
  uint32_t coefSign;                   /*!< Curve coefficient a sign */
  const uint8_t *coefA;                /*!< Pointer to curve coefficient |a| (Array of modulusSize elements) */
  const uint8_t *coefB;                /*!< Pointer to curve coefficient b   (Array of modulusSize elements) */
  const uint8_t *modulus;              /*!< Pointer to curve modulus value p (Array of modulusSize elements) */
  const uint8_t *pointX;               /*!< Pointer to point P coordinate xP (Array of modulusSize elements) */
  const uint8_t *pointY;               /*!< Pointer to point P coordinate yP (Array of modulusSize elements) */
  const uint32_t *pMontgomeryParam;    /*!< pointer to montgomery param R2 (modulus N) */
} PKA_PointCheckInTypeDef;

typedef struct
{
  uint32_t size;                       /*!< Number of element in popA array */
  const uint8_t *pOpDp;                /*!< Pointer to operand dP   (Array of size/2 elements) */
  const uint8_t *pOpDq;                /*!< Pointer to operand dQ   (Array of size/2 elements) */
  const uint8_t *pOpQinv;              /*!< Pointer to operand qinv (Array of size/2 elements) */
  const uint8_t *pPrimeP;              /*!< Pointer to prime p      (Array of size/2 elements) */
  const uint8_t *pPrimeQ;              /*!< Pointer to prime Q      (Array of size/2 elements) */
  const uint8_t *popA;                 /*!< Pointer to operand A    (Array of size elements) */
} PKA_RSACRTExpInTypeDef;

typedef struct
{
  uint32_t primeOrderSize;             /*!< Number of element in primeOrder array */
  uint32_t modulusSize;                /*!< Number of element in modulus array */
  uint32_t coefSign;                   /*!< Curve coefficient a sign */
  const uint8_t *coef;                 /*!< Pointer to curve coefficient |a|     (Array of modulusSize elements) */
  const uint8_t *modulus;              /*!< Pointer to curve modulus value p     (Array of modulusSize elements) */
  const uint8_t *basePointX;           /*!< Pointer to curve base point xG       (Array of modulusSize elements) */
  const uint8_t *basePointY;           /*!< Pointer to curve base point yG       (Array of modulusSize elements) */
  const uint8_t *pPubKeyCurvePtX;      /*!< Pointer to public-key curve point xQ (Array of modulusSize elements) */
  const uint8_t *pPubKeyCurvePtY;      /*!< Pointer to public-key curve point yQ (Array of modulusSize elements) */
  const uint8_t *RSign;                /*!< Pointer to signature part r          (Array of primeOrderSize elements) */
  const uint8_t *SSign;                /*!< Pointer to signature part s          (Array of primeOrderSize elements) */
  const uint8_t *hash;                 /*!< Pointer to hash of the message e     (Array of primeOrderSize elements) */
  const uint8_t *primeOrder;           /*!< Pointer to order of the curve n      (Array of primeOrderSize elements) */
} PKA_ECDSAVerifInTypeDef;

typedef struct
{
  uint32_t primeOrderSize;             /*!< Number of element in primeOrder array */
  uint32_t modulusSize;                /*!< Number of element in modulus array */
  uint32_t coefSign;                   /*!< Curve coefficient a sign */
  const uint8_t *coef;                 /*!< Pointer to curve coefficient |a|     (Array of modulusSize elements) */
  const uint8_t *coefB;                /*!< Pointer to B coefficient             (Array of modulusSize elements) */
  const uint8_t *modulus;              /*!< Pointer to curve modulus value p     (Array of modulusSize elements) */
  const uint8_t *integer;              /*!< Pointer to random integer k          (Array of primeOrderSize elements) */
  const uint8_t *basePointX;           /*!< Pointer to curve base point xG       (Array of modulusSize elements) */
  const uint8_t *basePointY;           /*!< Pointer to curve base point yG       (Array of modulusSize elements) */
  const uint8_t *hash;                 /*!< Pointer to hash of the message       (Array of primeOrderSize elements) */
  const uint8_t *privateKey;           /*!< Pointer to private key d             (Array of primeOrderSize elements) */
  const uint8_t *primeOrder;           /*!< Pointer to order of the curve n      (Array of primeOrderSize elements) */
} PKA_ECDSASignInTypeDef;

typedef struct
{
  uint8_t *RSign;                      /*!< Pointer to signature part r          (Array of modulusSize elements) */
  uint8_t *SSign;                      /*!< Pointer to signature part s          (Array of modulusSize elements) */
} PKA_ECDSASignOutTypeDef;

typedef struct
{
  uint8_t *ptX;                        /*!< Pointer to point P coordinate xP     (Array of modulusSize elements) */
  uint8_t *ptY;                        /*!< Pointer to point P coordinate yP     (Array of modulusSize elements) */
} PKA_ECDSASignOutExtParamTypeDef, PKA_ECCMulOutTypeDef, PKA_ECCProjective2AffineOutTypeDef,
PKA_ECCDoubleBaseLadderOutTypeDef;

typedef struct
{
  uint8_t *ptX;                        /*!< pointer to point P coordinate xP */
  uint8_t *ptY;                        /*!< pointer to point P coordinate yP */
  uint8_t *ptZ;                        /*!< pointer to point P coordinate zP */
} PKA_ECCCompleteAdditionOutTypeDef;

typedef struct
{
  uint32_t expSize;                    /*!< Number of element in pExp array */
  uint32_t OpSize;                     /*!< Number of element in pOp1 and pMod arrays */
  const uint8_t *pExp;                 /*!< Pointer to Exponent             (Array of expSize elements) */
  const uint8_t *pOp1;                 /*!< Pointer to Operand              (Array of OpSize elements) */
  const uint8_t *pMod;                 /*!< Pointer to modulus              (Array of OpSize elements) */
} PKA_ModExpInTypeDef;

typedef struct
{
  uint32_t expSize;                    /*!< Size of the operand in bytes */
  uint32_t OpSize;                     /*!< Size of the operand in bytes */
  const uint8_t *pOp1;                 /*!< Pointer to Operand 1 */
  const uint8_t *pExp;                 /*!< Pointer to Exponent */
  const uint8_t *pMod;                 /*!< Pointer to Operand 1 */
  const uint8_t *pPhi;                 /*!< Pointer to Phi value */
} PKA_ModExpProtectModeInTypeDef;

typedef struct
{
  uint32_t expSize;                    /*!< Number of element in pExp and pMontgomeryParam arrays */
  uint32_t OpSize;                     /*!< Number of element in pOp1 and pMod arrays */
  const uint8_t *pExp;                 /*!< Pointer to Exponent             (Array of expSize elements) */
  const uint8_t *pOp1;                 /*!< Pointer to Operand              (Array of OpSize elements) */
  const uint8_t *pMod;                 /*!< Pointer to modulus              (Array of OpSize elements) */
  const uint32_t *pMontgomeryParam;    /*!< Pointer to Montgomery parameter (Array of expSize/4 elements) */
} PKA_ModExpFastModeInTypeDef;

typedef struct
{
  uint32_t size;                       /*!< Number of element in pOp1 array */
  const uint8_t *pOp1;                 /*!< Pointer to Operand (Array of size elements) */
} PKA_MontgomeryParamInTypeDef;

typedef struct
{
  uint32_t size;                       /*!< Number of element in pOp1 and pOp2 arrays */
  const uint32_t *pOp1;                /*!< Pointer to Operand 1 (Array of size elements) */
  const uint32_t *pOp2;                /*!< Pointer to Operand 2 (Array of size elements) */
} PKA_AddInTypeDef, PKA_SubInTypeDef, PKA_MulInTypeDef, PKA_CmpInTypeDef;

typedef struct
{
  uint32_t size;                       /*!< Number of element in pOp1 array */
  const uint32_t *pOp1;                /*!< Pointer to Operand 1       (Array of size elements) */
  const uint8_t *pMod;                 /*!< Pointer to modulus value n (Array of size*4 elements) */
} PKA_ModInvInTypeDef;

typedef struct
{
  uint32_t OpSize;                     /*!< Number of element in pOp1 array */
  uint32_t modSize;                    /*!< Number of element in pMod array */
  const uint32_t *pOp1;                /*!< Pointer to Operand 1       (Array of OpSize elements) */
  const uint8_t *pMod;                 /*!< Pointer to modulus value n (Array of modSize elements) */
} PKA_ModRedInTypeDef;

typedef struct
{
  uint32_t size;                       /*!< Number of element in pOp1 and pOp2 arrays */
  const uint32_t *pOp1;                /*!< Pointer to Operand 1 (Array of size elements) */
  const uint32_t *pOp2;                /*!< Pointer to Operand 2 (Array of size elements) */
  const uint8_t  *pOp3;                /*!< Pointer to Operand 3 (Array of size*4 elements) */
} PKA_ModAddInTypeDef, PKA_ModSubInTypeDef, PKA_MontgomeryMulInTypeDef;

typedef struct
{
  uint32_t primeOrderSize;             /*!< curve prime order n length */
  uint32_t modulusSize;                /*!< curve modulus p length */
  uint32_t coefSign;                   /*!< curve coefficient a sign */
  const uint8_t *coefA;                /*!< pointer to curve coefficient |a| */
  const uint8_t *modulus;              /*!< pointer to curve modulus value p */
  const uint8_t *integerK;             /*!< pointer to cryptographically secure random integer k */
  const uint8_t *integerM;             /*!< pointer to cryptographically secure random integer m */
  const uint8_t *basePointX1;          /*!< pointer to curve base first point coordinate x */
  const uint8_t *basePointY1;          /*!< pointer to curve base first point coordinate y */
  const uint8_t *basePointZ1;          /*!< pointer to curve base first point coordinate z */
  const uint8_t *basePointX2;          /*!< pointer to curve base second point coordinate x */
  const uint8_t *basePointY2;          /*!< pointer to curve base second point coordinate y */
  const uint8_t *basePointZ2;          /*!< pointer to curve base second point coordinate z */
} PKA_ECCDoubleBaseLadderInTypeDef;

typedef struct
{
  uint32_t modulusSize;                /*!< curve modulus p length */
  const uint8_t *modulus;              /*!< pointer to curve modulus value p */
  const uint8_t *basePointX;           /*!< pointer to curve base point coordinate x */
  const uint8_t *basePointY;           /*!< pointer to curve base point coordinate y */
  const uint8_t *basePointZ;           /*!< pointer to curve base point coordinate z */
  const uint32_t *pMontgomeryParam;    /*!< pointer to montgomery parameter R2 modulus n*/
} PKA_ECCProjective2AffineInTypeDef;

typedef struct
{
  uint32_t modulusSize;                /*!< curve modulus p length */
  uint32_t coefSign;                   /*!< curve coefficient a sign */
  const uint8_t *modulus;              /*!< pointer to curve modulus value p */
  const uint8_t *coefA;                /*!< pointer to curve coefficient |a| */
  const uint8_t *basePointX1;          /*!< pointer to curve base first point coordinate x */
  const uint8_t *basePointY1;          /*!< pointer to curve base first point coordinate y */
  const uint8_t *basePointZ1;          /*!< pointer to curve base first point coordinate z */
  const uint8_t *basePointX2;          /*!< pointer to curve base second point coordinate x */
  const uint8_t *basePointY2;          /*!< pointer to curve base second point coordinate y */
  const uint8_t *basePointZ2;          /*!< pointer to curve base second point coordinate z */
} PKA_ECCCompleteAdditionInTypeDef;
/**
  * @}
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup PKA_Exported_Constants PKA Exported Constants
  * @{
  */

/** @defgroup PKA_Mode PKA mode
  * @{
  */
#define PKA_MODE_MONTGOMERY_PARAM                 (0x00000001U)
#define PKA_MODE_MODULAR_EXP                      (0x00000000U)
#define PKA_MODE_MODULAR_EXP_FAST_MODE            (0x00000002U)
#define PKA_MODE_ECC_MUL                          (0x00000020U)
#define PKA_MODE_ECDSA_SIGNATURE                  (0x00000024U)
#define PKA_MODE_ECDSA_VERIFICATION               (0x00000026U)
#define PKA_MODE_POINT_CHECK                      (0x00000028U)
#define PKA_MODE_RSA_CRT_EXP                      (0x00000007U)
#define PKA_MODE_MODULAR_INV                      (0x00000008U)
#define PKA_MODE_ARITHMETIC_ADD                   (0x00000009U)
#define PKA_MODE_ARITHMETIC_SUB                   (0x0000000AU)
#define PKA_MODE_ARITHMETIC_MUL                   (0x0000000BU)
#define PKA_MODE_COMPARISON                       (0x0000000CU)
#define PKA_MODE_MODULAR_RED                      (0x0000000DU)
#define PKA_MODE_MODULAR_ADD                      (0x0000000EU)
#define PKA_MODE_MODULAR_SUB                      (0x0000000FU)
#define PKA_MODE_MONTGOMERY_MUL                   (0x00000010U)
#define PKA_MODE_ECC_PROJECTIVE_AFF               (0x0000002FU)
#define PKA_MODE_DOUBLE_BASE_LADDER               (0x00000027U)
#define PKA_MODE_ECC_COMPLETE_ADD                 (0x00000023U)
#define PKA_MODE_MODULAR_EXP_PROTECT              (0x00000003U)
/**
  * @}
  */

/** @defgroup PKA_Interrupt_configuration_definition PKA Interrupt configuration definition
  * @brief PKA Interrupt definition
  * @{
  */
#define PKA_IT_PROCEND                            PKA_CR_PROCENDIE
#define PKA_IT_ADDRERR                            PKA_CR_ADDRERRIE
#define PKA_IT_RAMERR                             PKA_CR_RAMERRIE
#define PKA_IT_OPERR                              PKA_CR_OPERRIE

/**
  * @}
  */

/** @defgroup PKA_Flag_definition PKA Flag definition
  * @{
  */
#define PKA_FLAG_PROCEND                          PKA_SR_PROCENDF
#define PKA_FLAG_ADDRERR                          PKA_SR_ADDRERRF
#define PKA_FLAG_RAMERR                           PKA_SR_RAMERRF
#define PKA_FLAG_OPERR                            PKA_SR_OPERRF

/**
  * @}
  */

/** @defgroup PKA_Operation_Status PKA Operation Status
  * @{
  */
#define PKA_NO_ERROR                           0xD60DUL
#define PKA_FAILED_COMPUTATION                 0xCBC9UL
#define PKA_RPART_SIGNATURE_NULL               0xA3B7UL
#define PKA_SPART_SIGNATURE_NULL               0xF946UL

/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/

/** @defgroup PKA_Exported_Macros PKA Exported Macros
  * @{
  */

/** @brief  Reset PKA handle state.
  * @param  __HANDLE__ specifies the PKA Handle
  * @retval None
  */
#if (USE_HAL_PKA_REGISTER_CALLBACKS == 1)
#define __HAL_PKA_RESET_HANDLE_STATE(__HANDLE__)                do{                                                   \
                                                                    (__HANDLE__)->State = HAL_PKA_STATE_RESET;       \
                                                                    (__HANDLE__)->MspInitCallback = NULL;            \
                                                                    (__HANDLE__)->MspDeInitCallback = NULL;          \
                                                                  } while(0)
#else
#define __HAL_PKA_RESET_HANDLE_STATE(__HANDLE__)                ((__HANDLE__)->State = HAL_PKA_STATE_RESET)
#endif /* USE_HAL_PKA_REGISTER_CALLBACKS */

/** @brief  Enable the specified PKA interrupt.
  * @param  __HANDLE__ specifies the PKA Handle
  * @param  __INTERRUPT__ specifies the interrupt source to enable.
  *        This parameter can be one of the following values:
  *            @arg @ref PKA_IT_PROCEND End Of Operation interrupt enable
  *            @arg @ref PKA_IT_ADDRERR Address error interrupt enable
  *            @arg @ref PKA_IT_RAMERR RAM error interrupt enable
  *            @arg @ref PKA_IT_OPERR Operation error interrupt enable
  * @retval None
  */
#define __HAL_PKA_ENABLE_IT(__HANDLE__, __INTERRUPT__)          ((__HANDLE__)->Instance->CR |= (__INTERRUPT__))

/** @brief  Disable the specified PKA interrupt.
  * @param  __HANDLE__ specifies the PKA Handle
  * @param  __INTERRUPT__ specifies the interrupt source to disable.
  *        This parameter can be one of the following values:
  *            @arg @ref PKA_IT_PROCEND End Of Operation interrupt enable
  *            @arg @ref PKA_IT_ADDRERR Address error interrupt enable
  *            @arg @ref PKA_IT_RAMERR RAM error interrupt enable
  *            @arg @ref PKA_IT_OPERR Operation error interrupt enable
  * @retval None
  */
#define __HAL_PKA_DISABLE_IT(__HANDLE__, __INTERRUPT__)         ((__HANDLE__)->Instance->CR &= (~(__INTERRUPT__)))

/** @brief  Check whether the specified PKA interrupt source is enabled or not.
  * @param  __HANDLE__ specifies the PKA Handle
  * @param  __INTERRUPT__ specifies the PKA interrupt source to check.
  *        This parameter can be one of the following values:
  *            @arg @ref PKA_IT_PROCEND End Of Operation interrupt enable
  *            @arg @ref PKA_IT_ADDRERR Address error interrupt enable
  *            @arg @ref PKA_IT_RAMERR RAM error interrupt enable
  *            @arg @ref PKA_IT_OPERR Operation error interrupt enable
  * @retval The new state of __INTERRUPT__ (SET or RESET)
  */
#define __HAL_PKA_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)      ((((__HANDLE__)->Instance->CR\
                                                                   & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Check whether the specified PKA flag is set or not.
  * @param  __HANDLE__ specifies the PKA Handle
  * @param  __FLAG__ specifies the flag to check.
  *        This parameter can be one of the following values:
  *            @arg @ref PKA_FLAG_PROCEND End Of Operation
  *            @arg @ref PKA_FLAG_ADDRERR Address error
  *            @arg @ref PKA_FLAG_RAMERR RAM error
  *            @arg @ref PKA_FLAG_OPERR Operation error
  * @retval The new state of __FLAG__ (SET or RESET)
  */
#define __HAL_PKA_GET_FLAG(__HANDLE__, __FLAG__)                (((((__HANDLE__)->Instance->SR)\
                                                                   & (__FLAG__)) == (__FLAG__)) ? SET : RESET)

/** @brief  Clear the PKA pending flags which are cleared by writing 1 in a specific bit.
  * @param  __HANDLE__ specifies the PKA Handle
  * @param  __FLAG__ specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg @ref PKA_FLAG_PROCEND End Of Operation
  *            @arg @ref PKA_FLAG_ADDRERR Address error
  *            @arg @ref PKA_FLAG_RAMERR RAM error
  *            @arg @ref PKA_FLAG_OPERR Operation error
  * @retval None
  */
#define __HAL_PKA_CLEAR_FLAG(__HANDLE__, __FLAG__)              ((__HANDLE__)->Instance->CLRFR = (__FLAG__))

/** @brief  Enable the specified PKA peripheral.
  * @param  __HANDLE__ specifies the PKA Handle
  * @retval None
  */
#define __HAL_PKA_ENABLE(__HANDLE__)                            (SET_BIT((__HANDLE__)->Instance->CR,  PKA_CR_EN))

/** @brief  Disable the specified PKA peripheral.
  * @param  __HANDLE__ specifies the PKA Handle
  * @retval None
  */
#define __HAL_PKA_DISABLE(__HANDLE__)                           (CLEAR_BIT((__HANDLE__)->Instance->CR, PKA_CR_EN))

/** @brief  Start a PKA operation.
  * @param  __HANDLE__ specifies the PKA Handle
  * @retval None
  */
#define __HAL_PKA_START(__HANDLE__)                             (SET_BIT((__HANDLE__)->Instance->CR,  PKA_CR_START))
/**
  * @}
  */

/* Private macros --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @addtogroup PKA_Exported_Functions
  * @{
  */

/** @addtogroup PKA_Exported_Functions_Group1
  * @{
  */
/* Initialization and de-initialization functions *****************************/
HAL_StatusTypeDef HAL_PKA_Init(PKA_HandleTypeDef *hpka);
HAL_StatusTypeDef HAL_PKA_DeInit(PKA_HandleTypeDef *hpka);
void              HAL_PKA_MspInit(PKA_HandleTypeDef *hpka);
void              HAL_PKA_MspDeInit(PKA_HandleTypeDef *hpka);

#if (USE_HAL_PKA_REGISTER_CALLBACKS == 1)
/* Callbacks Register/UnRegister functions  ***********************************/
HAL_StatusTypeDef HAL_PKA_RegisterCallback(PKA_HandleTypeDef *hpka, HAL_PKA_CallbackIDTypeDef CallbackID,
                                           pPKA_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_PKA_UnRegisterCallback(PKA_HandleTypeDef *hpka, HAL_PKA_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_PKA_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @addtogroup PKA_Exported_Functions_Group2
  * @{
  */
/* IO operation functions *****************************************************/
/* High Level Functions *******************************************************/
HAL_StatusTypeDef HAL_PKA_ModExp(PKA_HandleTypeDef *hpka, PKA_ModExpInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModExp_IT(PKA_HandleTypeDef *hpka, PKA_ModExpInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ModExpFastMode(PKA_HandleTypeDef *hpka, PKA_ModExpFastModeInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModExpFastMode_IT(PKA_HandleTypeDef *hpka, PKA_ModExpFastModeInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ModExpProtectMode(PKA_HandleTypeDef *hpka, PKA_ModExpProtectModeInTypeDef *in,
                                            uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModExpProtectMode_IT(PKA_HandleTypeDef *hpka, PKA_ModExpProtectModeInTypeDef *in);
void HAL_PKA_ModExp_GetResult(PKA_HandleTypeDef *hpka, uint8_t *pRes);

HAL_StatusTypeDef HAL_PKA_ECDSASign(PKA_HandleTypeDef *hpka, PKA_ECDSASignInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECDSASign_IT(PKA_HandleTypeDef *hpka, PKA_ECDSASignInTypeDef *in);
void HAL_PKA_ECDSASign_GetResult(PKA_HandleTypeDef *hpka, PKA_ECDSASignOutTypeDef *out,
                                 PKA_ECDSASignOutExtParamTypeDef *outExt);

HAL_StatusTypeDef HAL_PKA_ECDSAVerif(PKA_HandleTypeDef *hpka, PKA_ECDSAVerifInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECDSAVerif_IT(PKA_HandleTypeDef *hpka, PKA_ECDSAVerifInTypeDef *in);
uint32_t HAL_PKA_ECDSAVerif_IsValidSignature(PKA_HandleTypeDef const *const hpka);

HAL_StatusTypeDef HAL_PKA_RSACRTExp(PKA_HandleTypeDef *hpka, PKA_RSACRTExpInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_RSACRTExp_IT(PKA_HandleTypeDef *hpka, PKA_RSACRTExpInTypeDef *in);
void HAL_PKA_RSACRTExp_GetResult(PKA_HandleTypeDef *hpka, uint8_t *pRes);

HAL_StatusTypeDef HAL_PKA_PointCheck(PKA_HandleTypeDef *hpka, PKA_PointCheckInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_PointCheck_IT(PKA_HandleTypeDef *hpka, PKA_PointCheckInTypeDef *in);
uint32_t HAL_PKA_PointCheck_IsOnCurve(PKA_HandleTypeDef const *const hpka);

HAL_StatusTypeDef HAL_PKA_ECCMul(PKA_HandleTypeDef *hpka, PKA_ECCMulInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECCMul_IT(PKA_HandleTypeDef *hpka, PKA_ECCMulInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ECCMulEx(PKA_HandleTypeDef *hpka, PKA_ECCMulExInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECCMulEx_IT(PKA_HandleTypeDef *hpka, PKA_ECCMulExInTypeDef *in);
void HAL_PKA_ECCMul_GetResult(PKA_HandleTypeDef *hpka, PKA_ECCMulOutTypeDef *out);

HAL_StatusTypeDef HAL_PKA_Add(PKA_HandleTypeDef *hpka, PKA_AddInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_Add_IT(PKA_HandleTypeDef *hpka, PKA_AddInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_Sub(PKA_HandleTypeDef *hpka, PKA_SubInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_Sub_IT(PKA_HandleTypeDef *hpka, PKA_SubInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_Cmp(PKA_HandleTypeDef *hpka, PKA_CmpInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_Cmp_IT(PKA_HandleTypeDef *hpka, PKA_CmpInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_Mul(PKA_HandleTypeDef *hpka, PKA_MulInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_Mul_IT(PKA_HandleTypeDef *hpka, PKA_MulInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ModAdd(PKA_HandleTypeDef *hpka, PKA_ModAddInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModAdd_IT(PKA_HandleTypeDef *hpka, PKA_ModAddInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ModSub(PKA_HandleTypeDef *hpka, PKA_ModSubInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModSub_IT(PKA_HandleTypeDef *hpka, PKA_ModSubInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ModInv(PKA_HandleTypeDef *hpka, PKA_ModInvInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModInv_IT(PKA_HandleTypeDef *hpka, PKA_ModInvInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ModRed(PKA_HandleTypeDef *hpka, PKA_ModRedInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModRed_IT(PKA_HandleTypeDef *hpka, PKA_ModRedInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_MontgomeryMul(PKA_HandleTypeDef *hpka, PKA_MontgomeryMulInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_MontgomeryMul_IT(PKA_HandleTypeDef *hpka, PKA_MontgomeryMulInTypeDef *in);
void HAL_PKA_Arithmetic_GetResult(PKA_HandleTypeDef *hpka, uint32_t *pRes);

HAL_StatusTypeDef HAL_PKA_MontgomeryParam(PKA_HandleTypeDef *hpka, PKA_MontgomeryParamInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_MontgomeryParam_IT(PKA_HandleTypeDef *hpka, PKA_MontgomeryParamInTypeDef *in);
void HAL_PKA_MontgomeryParam_GetResult(PKA_HandleTypeDef *hpka, uint32_t *pRes);

HAL_StatusTypeDef HAL_PKA_ECCDoubleBaseLadder(PKA_HandleTypeDef *hpka, PKA_ECCDoubleBaseLadderInTypeDef *in,
                                              uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECCDoubleBaseLadder_IT(PKA_HandleTypeDef *hpka, PKA_ECCDoubleBaseLadderInTypeDef *in);
void HAL_PKA_ECCDoubleBaseLadder_GetResult(PKA_HandleTypeDef *hpka, PKA_ECCDoubleBaseLadderOutTypeDef *out);

HAL_StatusTypeDef HAL_PKA_ECCProjective2Affine(PKA_HandleTypeDef *hpka, PKA_ECCProjective2AffineInTypeDef *in,
                                               uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECCProjective2Affine_IT(PKA_HandleTypeDef *hpka, PKA_ECCProjective2AffineInTypeDef *in);
void HAL_PKA_ECCProjective2Affine_GetResult(PKA_HandleTypeDef *hpka, PKA_ECCProjective2AffineOutTypeDef *out);

HAL_StatusTypeDef HAL_PKA_ECCCompleteAddition(PKA_HandleTypeDef *hpka, PKA_ECCCompleteAdditionInTypeDef *in,
                                              uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECCCompleteAddition_IT(PKA_HandleTypeDef *hpka, PKA_ECCCompleteAdditionInTypeDef *in);
void HAL_PKA_ECCCompleteAddition_GetResult(PKA_HandleTypeDef *hpka, PKA_ECCCompleteAdditionOutTypeDef *out);

HAL_StatusTypeDef HAL_PKA_Abort(PKA_HandleTypeDef *hpka);
void HAL_PKA_RAMReset(PKA_HandleTypeDef *hpka);
void HAL_PKA_OperationCpltCallback(PKA_HandleTypeDef *hpka);
void HAL_PKA_ErrorCallback(PKA_HandleTypeDef *hpka);
void HAL_PKA_IRQHandler(PKA_HandleTypeDef *hpka);
/**
  * @}
  */

/** @addtogroup PKA_Exported_Functions_Group3
  * @{
  */
/* Peripheral State and Error functions ***************************************/
HAL_PKA_StateTypeDef HAL_PKA_GetState(const PKA_HandleTypeDef *hpka);
uint32_t             HAL_PKA_GetError(const PKA_HandleTypeDef *hpka);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* defined(PKA) && defined(HAL_PKA_MODULE_ENABLED) */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_PKA_H */
