/**
  ******************************************************************************
  * @file    stm32n6xx_hal_bsec.h
  * @author  MCD Application Team
  * @brief   Header file of BSEC HAL module.
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
#ifndef STM32N6xx_HAL_BSEC_H
#define STM32N6xx_HAL_BSEC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
 * @{
 */

/** @addtogroup BSEC
 * @{
 */

/* Exported types ------------------------------------------------------------*/
/** @defgroup BSEC_Exported_Types BSEC Exported Types
 * @{
 */

/**
  * @brief  HAL BSEC Handle Structure definition
  */
typedef struct
{
  BSEC_TypeDef *Instance; /*!< BSEC registers base address                     */
  uint32_t     ErrorCode; /*!< Error code in case of HAL driver internal error */
} BSEC_HandleTypeDef;

/**
  * @brief  HAL BSEC Scratch and Write Once Scratch Register Structure definition
  */
typedef struct
{
  uint32_t RegType;    /*!< Type of the register
                            This parameter can be a value of @ref BSEC_Register_Type                         */
  uint32_t RegNumber;  /*!< Number of the register
                            This parameter can be a value between 0 and 3 if RegType is HAL_BSEC_SCRATCH_REG
                            and between 0 and 7 if RegType is HAL_BSEC_WRITE_ONCE_SCRATCH_REG                */
} BSEC_ScratchRegTypeDef;

/**
  * @brief  HAL BSEC Debug configuration Structure definition
  */
typedef struct
{
  uint32_t HDPL_Open_Dbg;    /*!< Level at which debug may be opened
                                  This parameter can be a value of @ref BSEC_Open_Dbg        */
  uint32_t Sec_Dbg_Auth;     /*!< Secure debug authorization
                                  This parameter can be a value of @ref BSEC_Sec_Dbg_Auth    */
  uint32_t NonSec_Dbg_Auth;  /*!< Non-secure debug authorization
                                  This parameter can be a value of @ref BSEC_NonSec_Dbg_Auth */
} BSEC_DebugCfgTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup BSEC_Exported_Constants BSEC Exported Constants
  * @{
  */

/** @defgroup BSEC_Error_Code BSEC Error code
  * @{
  */
#define HAL_BSEC_ERROR_NONE             (0U)         /*!< No error                                 */
#define HAL_BSEC_ERROR_INVALID_PARAM    (1U)         /*!< Parameter error                          */
#define HAL_BSEC_ERROR_UNDEFINED_VALUE  (2U)         /*!< Undefined value error                    */
#define HAL_BSEC_ERROR_LOCK             (4U)         /*!< Locked operation error                   */
#define HAL_BSEC_ERROR_UNALLOWED        (8U)         /*!< Not possible operation error             */
#define HAL_BSEC_ERROR_TIMEOUT          (10U)        /*!< Timeout error                            */

#define HAL_BSEC_ERROR_PROGFAIL         BSEC_OTPSR_PROGFAIL  /*!< Programming failed                       */
#define HAL_BSEC_ERROR_DISTURB          BSEC_OTPSR_DISTURBF  /*!< Disturb flag (unexpected error)          */
#define HAL_BSEC_ERROR_DED              BSEC_OTPSR_DEDF      /*!< Double error detection flag              */
#define HAL_BSEC_ERROR_SEC              BSEC_OTPSR_SECF      /*!< Single error detection flag              */
#define HAL_BSEC_ERROR_PPL              BSEC_OTPSR_PPLF      /*!< Permanent programming lock flag          */
#define HAL_BSEC_ERROR_PPLM             BSEC_OTPSR_PPLMF     /*!< Permanent programming lock mismatch flag */
#define HAL_BSEC_ERROR_AME              BSEC_OTPSR_AMEF      /*!< Addresses mismatch error flag            */
/**
  * @}
  */

/** @defgroup BSEC_Global_Lock BSEC Global write lock
  * @{
  */
#define HAL_BSEC_WRITE_REG_ALLOWED  (0U)      /*!< Write to BSEC registers are allowed */
#define HAL_BSEC_WRITE_REG_IGNORED  BSEC_LOCKR_GWLOCK  /*!< Write to BSEC registers are ignored */
/**
  * @}
  */

/** @defgroup BSEC_Status BSEC Peripheral status
  * @{
  */
#define HAL_BSEC_INIT_DONE           BSEC_OTPSR_INIT_DONE  /*!< BSEC initialized following a cold or warm reset   */
#define HAL_BSEC_UPPER_FUSES_HIDDEN  BSEC_OTPSR_HIDEUP     /*!< Upper fuses are not accessible to the application */
#define HAL_BSEC_OTP_NOT_VIRGIN      BSEC_OTPSR_OTPNVIR    /*!< BSEC is not virgin                                */
/**
  * @}
  */

/** @defgroup BSEC_Permanent_Lock BSEC Permanent lock
  * @{
  */
#define HAL_BSEC_NORMAL_PROG  (0U)      /*!< Fuse word is programmed normally                      */
#define HAL_BSEC_LOCK_PROG    BSEC_OTPCR_PPLOCK  /*!< Fuse word is locked, preventing further modifications */
/**
  * @}
  */

/** @defgroup BSEC_Reload_Validity BSEC Validity of shadow register reload
  * @{
  */
#define HAL_BSEC_RELOAD_ERROR          (0U)  /*!< An error occurred while fuse word was last reloaded */
#define HAL_BSEC_RELOAD_DONE           (1U)  /*!< Last reload of fuse word was done without error     */
/**
  * @}
  */

/** @defgroup BSEC_State BSEC State
  * @{
  */
#define HAL_BSEC_FUSE_PROG_LOCKED      (0x00000001U)        /*!< Fuse word programming is silently ignored until next reset               */
#define HAL_BSEC_FUSE_WRITE_LOCKED     (0x00000002U)        /*!< Shadow register writing is silently ignored until next reset             */
#define HAL_BSEC_FUSE_RELOAD_LOCKED    (0x00000004U)        /*!< Fuse word loading is denied until next reset                             */
#define HAL_BSEC_FUSE_LOCKED           (0x00000008U)        /*!< Fuse word is locked, preventing further modifications                    */
#define HAL_BSEC_FUSE_SHADOWED         (0x00000100U)        /*!< Fuse word is shadowed                                                    */
#define HAL_BSEC_FUSE_HIDDEN           (0x00000200U)        /*!< Fuse word is hidden                                                      */
#define HAL_BSEC_FUSE_ERROR_DETECTED   BSEC_OTPSR_OTPERR  /*!< At least one error is detected during reset operations                   */
#define HAL_BSEC_FUSE_ERROR_CORRECTED  BSEC_OTPSR_OTPSEC  /*!< At least one single-error correction is detected during reset operations */
/**
  * @}
  */

/** @defgroup BSEC_Lifecycle_State BSEC Device lifecycle state
  * @{
  */
#define HAL_BSEC_OPEN_STATE                 (0x16U << BSEC_SR_NVSTATE_Pos)  /*!< BSEC is in open state                                              */
#define HAL_BSEC_CLOSED_STATE               (0x0DU << BSEC_SR_NVSTATE_Pos)  /*!< BSEC is in closed state                                            */
#define HAL_BSEC_INVALID_STATE              (0x07U << BSEC_SR_NVSTATE_Pos)  /*!< BSEC is in invalid state                                           */
#define HAL_BSEC_INVALID_STATE_WITH_TAMPER  (0x23U << BSEC_SR_NVSTATE_Pos)  /*!< BSEC is in invalid state with an active confirmed tamper triggered */
/**
  * @}
  */

/** @defgroup BSEC_Epoch_Select BSEC Epoch selected register
  * @{
  */
#define HAL_BSEC_EPOCH_COUNTER_0  (0U)  /*!< Epoch counter 0 */
#define HAL_BSEC_EPOCH_COUNTER_1  (1U)  /*!< Epoch counter 1 */
/**
  * @}
  */

/** @defgroup BSEC_HDPL BSEC Hide protection level
  * @{
  */
#define HAL_BSEC_HDPL_VALUE_0  (0xB4U)  /*!< Level 0 */
#define HAL_BSEC_HDPL_VALUE_1  (0x51U)  /*!< Level 1 */
#define HAL_BSEC_HDPL_VALUE_2  (0x8AU)  /*!< Level 2 */
#define HAL_BSEC_HDPL_VALUE_3  (0x6FU)  /*!< Level 3 */
/**
  * @}
  */

/** @defgroup BSEC_INCR_HDPL BSEC Increment of Hide protection level sent to SAES
  * @{
  */
#define HAL_BSEC_HDPL_INCR_VALUE_0  (0U)  /*!< HDPL sent to SAES incremented by 0 */
#define HAL_BSEC_HDPL_INCR_VALUE_1  (1U)  /*!< HDPL sent to SAES incremented by 1 */
#define HAL_BSEC_HDPL_INCR_VALUE_2  (2U)  /*!< HDPL sent to SAES incremented by 2 */
#define HAL_BSEC_HDPL_INCR_VALUE_3  (3U)  /*!< HDPL sent to SAES incremented by 3 */
/**
  * @}
  */

/** @defgroup BSEC_Register_Type BSEC Type of scratch register
  * @{
  */
#define HAL_BSEC_SCRATCH_REG             (0U)  /*!< Type of register is scratch register            */
#define HAL_BSEC_WRITE_ONCE_SCRATCH_REG  (1U)  /*!< Type of register is write once scratch register */
/**
  * @}
  */

/** @defgroup BSEC_Debug_Req BSEC Debug request
  * @{
  */
#define HAL_BSEC_DBG_NOT_REQUESTED  (0U)            /*!< Host debugger is not requesting debug */
#define HAL_BSEC_DBG_REQUESTED      BSEC_SR_DBGREQ  /*!< Host debugger is requesting debug     */
/**
  * @}
  */

/** @defgroup BSEC_Open_Dbg BSEC Open debug level
  * @{
  */
#define HAL_BSEC_OPEN_DBG_LEVEL_0  (0x00B40000U)  /*!< Debug opened for HDPL0 */
#define HAL_BSEC_OPEN_DBG_LEVEL_1  (0x00510000U)  /*!< Debug opened for HDPL1 */
#define HAL_BSEC_OPEN_DBG_LEVEL_2  (0x008A0000U)  /*!< Debug opened for HDPL2 */
#define HAL_BSEC_OPEN_DBG_LEVEL_3  (0x006F0000U)  /*!< Debug opened for HDPL3 */
/**
  * @}
  */

/** @defgroup BSEC_Sec_Dbg_Auth BSEC Secure debug authorization
  * @{
  */
#define HAL_BSEC_SEC_DBG_UNAUTH  (0x00000000U)  /*!< Secure debug not authorized */
#define HAL_BSEC_SEC_DBG_AUTH    (0xB4000000U)  /*!< Secure debug authorized     */
/**
  * @}
  */

/** @defgroup BSEC_NonSec_Dbg_Auth BSEC Non-secure debug authorization
  * @{
  */
#define HAL_BSEC_NONSEC_DBG_UNAUTH  (0x00000000U)  /*!< Non-secure debug not authorized */
#define HAL_BSEC_NONSEC_DBG_AUTH    (0x0000B400U)  /*!< Non-secure debug authorized     */
/**
  * @}
  */

/** @defgroup BSEC_Debug_Lock BSEC Debug lock
  * @{
  */
#define HAL_BSEC_DEBUG_LOCKED    (0x00000000U)  /*!< DBG_MCU AP interface locked   */
#define HAL_BSEC_DEBUG_UNLOCKED  (0x000000B4U)  /*!< DBG_MCU AP interface unlocked */
/**
  * @}
  */

/** @defgroup BSEC_DHUK_Validity BSEC Derived hardware unique key validity
  * @{
  */
#define HAL_BSEC_DHUK_INVALID    (0U)            /*!< DHUK can't be used in SAES */
#define HAL_BSEC_DHUK_VALID      BSEC_SR_HVALID  /*!< DHUK can be used in SAES   */
/**
  * @}
  */

/** @defgroup BSEC_DHUK_Lock BSEC Derived hardware unique key lock
  * @{
  */
#define HAL_BSEC_DHUK_USABLE      (0U)               /*!< DHUK in SAES is usable */
#define HAL_BSEC_DHUK_NOT_USABLE  BSEC_LOCKR_HKLOCK  /*!< DHUK in SAES is not usable */
/**
  * @}
  */

/** @defgroup BSEC_Reset_Type BSEC Type of reset
  * @{
  */
#define HAL_BSEC_HOT_RESET   (0U)  /*!< Type of reset is hot reset */
#define HAL_BSEC_WARM_RESET  (1U)  /*!< Type of reset is warm reset */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/

/** @defgroup BSEC_Exported_Macros BSEC Exported Macros
  * @{
  */


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup BSEC_Exported_Functions
  * @{
  */

/** @addtogroup BSEC_Exported_Functions_Group1
  * @{
  */
HAL_StatusTypeDef HAL_BSEC_GetStatus(BSEC_HandleTypeDef * hbsec, uint32_t *pStatus);
HAL_StatusTypeDef HAL_BSEC_GlobalLock(BSEC_HandleTypeDef *hbsec);
HAL_StatusTypeDef HAL_BSEC_GetGlobalLockStatus(BSEC_HandleTypeDef * hbsec, uint32_t *pStatus);
HAL_StatusTypeDef HAL_BSEC_GetErrorCode(BSEC_HandleTypeDef * hbsec, uint32_t *pError);
/**
  * @}
  */

/** @addtogroup BSEC_Exported_Functions_Group2
  * @{
  */
HAL_StatusTypeDef HAL_BSEC_OTP_Read(BSEC_HandleTypeDef * hbsec, uint32_t FuseId, uint32_t *pFuseData);
HAL_StatusTypeDef HAL_BSEC_OTP_Program(BSEC_HandleTypeDef *hbsec, uint32_t FuseId, uint32_t FuseData, uint32_t Lock);
HAL_StatusTypeDef HAL_BSEC_OTP_Reload(BSEC_HandleTypeDef *hbsec, uint32_t FuseId);
HAL_StatusTypeDef HAL_BSEC_OTP_Lock(BSEC_HandleTypeDef *hbsec, uint32_t FuseId, uint32_t Lock);
HAL_StatusTypeDef HAL_BSEC_OTP_GetState(BSEC_HandleTypeDef * hbsec, uint32_t FuseId, uint32_t *pState);
/**
  * @}
  */

/** @addtogroup BSEC_Exported_Functions_Group3
  * @{
  */
HAL_StatusTypeDef HAL_BSEC_OTP_ReadShadow(BSEC_HandleTypeDef * hbsec, uint32_t RegId, uint32_t *pRegData);
HAL_StatusTypeDef HAL_BSEC_OTP_WriteShadow(BSEC_HandleTypeDef *hbsec, uint32_t RegId, uint32_t RegData);
HAL_StatusTypeDef HAL_BSEC_OTP_GetShadowState(BSEC_HandleTypeDef * hbsec, uint32_t RegId, uint32_t *pValidity);
/**
  * @}
  */

/** @addtogroup BSEC_Exported_Functions_Group4
  * @{
  */
HAL_StatusTypeDef HAL_BSEC_GetDeviceLifeCycleState(BSEC_HandleTypeDef * hbsec, uint32_t *pState);
HAL_StatusTypeDef HAL_BSEC_ReadEpochCounter(BSEC_HandleTypeDef * hbsec, uint32_t CounterId, uint32_t *pCounterData);
HAL_StatusTypeDef HAL_BSEC_SelectEpochCounter(BSEC_HandleTypeDef *hbsec, uint32_t SelectedCounter);
HAL_StatusTypeDef HAL_BSEC_GetEpochCounterSelection(BSEC_HandleTypeDef * hbsec, uint32_t *pSelectedCounter);
/**
  * @}
  */

/** @addtogroup BSEC_Exported_Functions_Group5
  * @{
  */
HAL_StatusTypeDef HAL_BSEC_GetHDPLValue(BSEC_HandleTypeDef * hbsec, uint32_t *pHDPL);
HAL_StatusTypeDef HAL_BSEC_IncrementHDPLValue(BSEC_HandleTypeDef *hbsec);
HAL_StatusTypeDef HAL_BSEC_ConfigSAESHDPLIncrementValue(BSEC_HandleTypeDef *hbsec, uint32_t Increment);
HAL_StatusTypeDef HAL_BSEC_GetSAESHDPLIncrementValue(BSEC_HandleTypeDef * hbsec, uint32_t *pIncrement);
/**
  * @}
  */

/** @addtogroup BSEC_Exported_Functions_Group6
  * @{
  */
HAL_StatusTypeDef HAL_BSEC_WriteScratchValue(BSEC_HandleTypeDef *hbsec, const BSEC_ScratchRegTypeDef *pRegAddr, uint32_t Value);
HAL_StatusTypeDef HAL_BSEC_ReadScratchValue(BSEC_HandleTypeDef * hbsec, const BSEC_ScratchRegTypeDef *pRegAddr, uint32_t *pValue);
/**
  * @}
  */

/** @addtogroup BSEC_Exported_Functions_Group7
  * @{
  */
HAL_StatusTypeDef HAL_BSEC_GetDebugRequest(BSEC_HandleTypeDef * hbsec, uint32_t *pDbgReq);
HAL_StatusTypeDef HAL_BSEC_SendJTAGData(BSEC_HandleTypeDef *hbsec, uint32_t Data);
HAL_StatusTypeDef HAL_BSEC_ReceiveJTAGData(BSEC_HandleTypeDef * hbsec, uint32_t *pData);
HAL_StatusTypeDef HAL_BSEC_ConfigDebug(BSEC_HandleTypeDef *hbsec,const BSEC_DebugCfgTypeDef *pCfg);
HAL_StatusTypeDef HAL_BSEC_GetDebugConfig(BSEC_HandleTypeDef * hbsec, BSEC_DebugCfgTypeDef *pDbgCfg);
HAL_StatusTypeDef HAL_BSEC_LockDebug(BSEC_HandleTypeDef *hbsec);
HAL_StatusTypeDef HAL_BSEC_UnlockDebug(BSEC_HandleTypeDef *hbsec);
HAL_StatusTypeDef HAL_BSEC_GetDebugLockState(BSEC_HandleTypeDef * hbsec, uint32_t *pStatus);
/**
  * @}
  */

/** @addtogroup BSEC_Exported_Functions_Group8
  * @{
  */
HAL_StatusTypeDef HAL_BSEC_GetDHUKValidity(BSEC_HandleTypeDef * hbsec, uint32_t *pValidity);
HAL_StatusTypeDef HAL_BSEC_LockDHUKUse(BSEC_HandleTypeDef *hbsec);
HAL_StatusTypeDef HAL_BSEC_GetDHUKLockStatus(BSEC_HandleTypeDef * hbsec, uint32_t *pStatus);
/**
  * @}
  */

/** @addtogroup BSEC_Exported_Functions_Group9
  * @{
  */
HAL_StatusTypeDef HAL_BSEC_GetNumberOfResets(BSEC_HandleTypeDef * hbsec, uint32_t ResetType, uint32_t *pResetNumber);
/**
  * @}
  */

/**
  * @}
  */

/* Private macros --------------------------------------------------------*/
/**
  @cond 0
  */
#define IS_BSEC_PERMANENT_LOCK(VALUE)  (((VALUE) == HAL_BSEC_NORMAL_PROG) || ((VALUE) == HAL_BSEC_LOCK_PROG))

#define IS_BSEC_LOCK_CFG(CFG)          ((CFG) <= (HAL_BSEC_FUSE_PROG_LOCKED | HAL_BSEC_FUSE_WRITE_LOCKED | HAL_BSEC_FUSE_RELOAD_LOCKED))

#define IS_BSEC_STATE(STATE)           (((STATE) == HAL_BSEC_OPEN_STATE)    || ((STATE) == HAL_BSEC_CLOSED_STATE) \
                                     || ((STATE) == HAL_BSEC_INVALID_STATE) || ((STATE) == HAL_BSEC_INVALID_STATE_WITH_TAMPER))

#define IS_BSEC_EPOCHSEL(VALUE)        (((VALUE) == HAL_BSEC_EPOCH_COUNTER_0) || ((VALUE) == HAL_BSEC_EPOCH_COUNTER_1))

#define IS_BSEC_HDPL(LEVEL)            (((LEVEL) == HAL_BSEC_HDPL_VALUE_0) || ((LEVEL) == HAL_BSEC_HDPL_VALUE_1) \
                                     || ((LEVEL) == HAL_BSEC_HDPL_VALUE_2) || ((LEVEL) == HAL_BSEC_HDPL_VALUE_3))

#define IS_BSEC_NEXTHDPL(VALUE)        (((VALUE) == HAL_BSEC_HDPL_INCR_VALUE_0) || ((VALUE) == HAL_BSEC_HDPL_INCR_VALUE_1) \
                                     || ((VALUE) == HAL_BSEC_HDPL_INCR_VALUE_2) || ((VALUE) == HAL_BSEC_HDPL_INCR_VALUE_3))

#define IS_BSEC_REGTYPE(TYPE)          (((TYPE) == HAL_BSEC_SCRATCH_REG) || ((TYPE) == HAL_BSEC_WRITE_ONCE_SCRATCH_REG))

#define IS_BSEC_OPENDBG(LEVEL)        (((LEVEL) == HAL_BSEC_OPEN_DBG_LEVEL_0) || ((LEVEL) == HAL_BSEC_OPEN_DBG_LEVEL_1) \
                                    || ((LEVEL) == HAL_BSEC_OPEN_DBG_LEVEL_2) || ((LEVEL) == HAL_BSEC_OPEN_DBG_LEVEL_3))

#define IS_BSEC_SECDBGAUTH(AUTH)      (((AUTH) == HAL_BSEC_SEC_DBG_UNAUTH) || ((AUTH) == HAL_BSEC_SEC_DBG_AUTH))

#define IS_BSEC_NSDBGAUTH(AUTH)       (((AUTH) == HAL_BSEC_NONSEC_DBG_UNAUTH) || ((AUTH) == HAL_BSEC_NONSEC_DBG_AUTH))

#define IS_BSEC_DEBUGLOCK(VALUE)      (((VALUE) == HAL_BSEC_DEBUG_LOCKED) || ((VALUE) == HAL_BSEC_DEBUG_UNLOCKED))

#define IS_BSEC_RESETTYPE(TYPE)       (((TYPE) == HAL_BSEC_HOT_RESET) || ((TYPE) == HAL_BSEC_WARM_RESET))
/**
  @endcond
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

#endif /* STM32N6xx_HAL_BSEC_H */
