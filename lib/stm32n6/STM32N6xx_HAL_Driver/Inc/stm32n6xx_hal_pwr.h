/**
  ******************************************************************************
  * @file    stm32n6xx_hal_pwr.h
  * @author  MCD Application Team
  * @brief   Header file of PWR HAL module.
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
#ifndef STM32N6xx_HAL_PWR_H
#define STM32N6xx_HAL_PWR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup PWR
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup PWR_Exported_Types PWR Exported Types
  * @{
  */

/**
  * @brief  PWR PVD configuration structure definition
  */
typedef struct
{
  uint32_t Mode;     /*!< Specifies the operating mode for the selected pins.
                          This parameter can be a value of @ref PWR_PVD_Mode. */
} PWR_PVDTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup PWR_Exported_Constants PWR Exported Constants
  * @{
  */

/** @defgroup PWR_PVD_Mode PWR PVD Mode
  * @{
  */
#define PWR_PVD_MODE_NORMAL                (0x00000000U)                       /*!< Basic mode is used                                        */
#define PWR_PVD_MODE_IT_RISING             (0x00010001U)                       /*!< Interrupt Mode with Rising edge trigger detection         */
#define PWR_PVD_MODE_IT_FALLING            (0x00010002U)                       /*!< Interrupt Mode with Falling edge trigger detection        */
#define PWR_PVD_MODE_IT_RISING_FALLING     (0x00010003U)                       /*!< Interrupt Mode with Rising/Falling edge trigger detection */
#define PWR_PVD_MODE_EVENT_RISING          (0x00020001U)                       /*!< Event Mode with Rising edge trigger detection             */
#define PWR_PVD_MODE_EVENT_FALLING         (0x00020002U)                       /*!< Event Mode with Falling edge trigger detection            */
#define PWR_PVD_MODE_EVENT_RISING_FALLING  (0x00020003U)                       /*!< Event Mode with Rising/Falling edge trigger detection     */
/**
  * @}
  */

/** @defgroup PWR_Regulator_state_in_LP_mode PWR Regulator state in SLEEP/STOP mode
  * @{
  */
/* define for interface compatibility purpose */
#define PWR_MAINREGULATOR_ON           (0U)
/**
  * @}
  */

/** @defgroup PWR_SLEEP_mode_entry PWR SLEEP mode entry
  * @{
  */
#define PWR_SLEEPENTRY_WFI              (1U)                                   /*!< Wait For Interruption instruction to enter Sleep mode */
#define PWR_SLEEPENTRY_WFE              (2U)                                   /*!< Wait For Event instruction to enter Sleep mode        */
#define PWR_SLEEPENTRY_WFE_NO_EVT_CLEAR (3U)                                   /*!< Wait For Event instruction to enter Sleep mode with no event clear */
/**
  * @}
  */

/** @defgroup PWR_STOP_mode_entry PWR STOP mode entry
  * @{
  */
#define PWR_STOPENTRY_WFI              (1U)                                    /*!< Wait For Interruption instruction to enter Stop mode */
#define PWR_STOPENTRY_WFE              (2U)                                    /*!< Wait For Event instruction to enter Stop mode        */
#define PWR_STOPENTRY_WFE_NO_EVT_CLEAR (3U)                                    /*!< Wait For Event instruction to enter Stop mode with no event clear */
/**
  * @}
  */

/** @defgroup PWR_ENABLE_WUP_Mask PWR Enable WUP Mask
  * @{
  */
#define  PWR_EWUP_MASK                 (0x00FF0F0FU)
/**
  * @}
  */

/** @defgroup PWR_WakeUp_Pins PWR Wake-Up Pins
  * @{
  */
/* High level and No pull (default configuration) */
#define PWR_WAKEUP_PIN4                PWR_WKUPEPR_WKUPEN4                     /*!< Wakeup pin 1 (with high level polarity) */
#define PWR_WAKEUP_PIN3                PWR_WKUPEPR_WKUPEN3                     /*!< Wakeup pin 2 (with high level polarity) */
#define PWR_WAKEUP_PIN2                PWR_WKUPEPR_WKUPEN2                     /*!< Wakeup pin 3 (with high level polarity) */
#define PWR_WAKEUP_PIN1                PWR_WKUPEPR_WKUPEN1                     /*!< Wakeup pin 4 (with high level polarity) */

/* High level and No pull */
#define PWR_WAKEUP_PIN4_HIGH           PWR_WKUPEPR_WKUPEN4                     /*!< Wakeup pin 1 (with high level polarity) */
#define PWR_WAKEUP_PIN3_HIGH           PWR_WKUPEPR_WKUPEN3                     /*!< Wakeup pin 2 (with high level polarity) */
#define PWR_WAKEUP_PIN2_HIGH           PWR_WKUPEPR_WKUPEN2                     /*!< Wakeup pin 3 (with high level polarity) */
#define PWR_WAKEUP_PIN1_HIGH           PWR_WKUPEPR_WKUPEN1                     /*!< Wakeup pin 4 (with high level polarity) */

/* Low level and No pull */
#define PWR_WAKEUP_PIN4_LOW            (PWR_WKUPEPR_WKUPP4 | PWR_WKUPEPR_WKUPEN4)  /*!< Wakeup pin 1 (with low level polarity) */
#define PWR_WAKEUP_PIN3_LOW            (PWR_WKUPEPR_WKUPP3 | PWR_WKUPEPR_WKUPEN3)  /*!< Wakeup pin 2 (with low level polarity) */
#define PWR_WAKEUP_PIN2_LOW            (PWR_WKUPEPR_WKUPP2 | PWR_WKUPEPR_WKUPEN2)  /*!< Wakeup pin 3 (with low level polarity) */
#define PWR_WAKEUP_PIN1_LOW            (PWR_WKUPEPR_WKUPP1 | PWR_WKUPEPR_WKUPEN1)  /*!< Wakeup pin 4 (with low level polarity) */
/**
  * @}
  */

/** @defgroup PWR_Flag_WUP PWR Flag WakeUp
  * @{
  */
#define PWR_FLAG_WKUP1                 PWR_WKUPCR_WKUPC1
#define PWR_FLAG_WKUP2                 PWR_WKUPCR_WKUPC2
#define PWR_FLAG_WKUP3                 PWR_WKUPCR_WKUPC3
#define PWR_FLAG_WKUP4                 PWR_WKUPCR_WKUPC4
#define PWR_FLAG_WKUP                  (PWR_FLAG_WKUP1 | PWR_WKUPCR_WKUPC2 | \
                                        PWR_FLAG_WKUP3 | PWR_WKUPCR_WKUPC4)
/**
  * @}
  */

/** @defgroup PWR_PIN_Polarity PWR Pin Polarity configuration
  * @{
  */
#define PWR_PIN_POLARITY_HIGH          (0x0U)
#define PWR_PIN_POLARITY_LOW           (0x1U)
/**
  * @}
  */

/** @defgroup PWR_PIN_Pull PWR Pin Pull configuration
  * @{
  */
#define PWR_PIN_NO_PULL                (0x0U)
#define PWR_PIN_PULL_UP                (0x1U)
#define PWR_PIN_PULL_DOWN              (0x2U)
/**
  * @}
  */

/** @defgroup PWR_Wakeup_Pins_Flags PWR Wakeup Pins Flags.
  * @{
  */
#define PWR_WAKEUP_FLAG1               PWR_WKUPSR_WKUPF1                       /*!< Wakeup flag on PA0  */
#define PWR_WAKEUP_FLAG2               PWR_WKUPSR_WKUPF2                       /*!< Wakeup flag on PA2  */
#define PWR_WAKEUP_FLAG3               PWR_WKUPSR_WKUPF3                       /*!< Wakeup flag on PC13 */
#define PWR_WAKEUP_FLAG4               PWR_WKUPSR_WKUPF4                       /*!< Wakeup flag on PD2  */

#define PWR_WAKEUP_FLAG_ALL           (PWR_WKUPSR_WKUPF1 | PWR_WKUPSR_WKUPF2 |\
                                       PWR_WKUPSR_WKUPF3 | PWR_WKUPSR_WKUPF4)
/**
  * @}
  */

/** @defgroup PWR_Items PWR Items
  * @{
  */

/* Use the PWR_SECCFGR bits definition offset for both Secure and Privilege because
   it is the same offset value for the same item */
#define PWR_ITEM_0                     PWR_SECCFGR_SEC0                        /*!< System supply configuration secure/privilege protection       */
#define PWR_ITEM_1                     PWR_SECCFGR_SEC1                        /*!< Programmable voltage detector secure/privilege protection     */
#define PWR_ITEM_2                     PWR_SECCFGR_SEC2                        /*!< VDDCORE monitor secure/privilege protection                   */
#define PWR_ITEM_3                     PWR_SECCFGR_SEC3                        /*!< TCM and FLEXMEM low power control secure/privilege protection */
#define PWR_ITEM_4                     PWR_SECCFGR_SEC4                        /*!< Voltage scaling selection secure/privilege protection         */
#define PWR_ITEM_5                     PWR_SECCFGR_SEC5                        /*!< Backup domain secure/privilege protection                     */
#define PWR_ITEM_6                     PWR_SECCFGR_SEC6                        /*!< CPU power control secure/privilege protection                 */
#define PWR_ITEM_7                     PWR_SECCFGR_SEC7                        /*!< Peripheral voltage monitor secure/privilege protection        */
#define PWR_ITEM_WKUP1                 PWR_SECCFGR_WKUPSEC1                    /*!< WKUP1 pin secure/privilege protection                         */
#define PWR_ITEM_WKUP2                 PWR_SECCFGR_WKUPSEC2                    /*!< WKUP2 pin secure/privilege protection                         */
#define PWR_ITEM_WKUP3                 PWR_SECCFGR_WKUPSEC3                    /*!< WKUP3 pin secure/privilege protection                         */
#define PWR_ITEM_WKUP4                 PWR_SECCFGR_WKUPSEC4                    /*!< WKUP4 pin secure/privilege protection                         */

#define PWR_ITEM_ALL                   (PWR_ITEM_0 | PWR_ITEM_1 | PWR_ITEM_2 | PWR_ITEM_3 | PWR_ITEM_4 | PWR_ITEM_5 | \
                                        PWR_ITEM_6 | PWR_ITEM_7 | PWR_ITEM_WKUP1 | PWR_ITEM_WKUP2 | PWR_ITEM_WKUP3 | \
                                        PWR_ITEM_WKUP4)
/**
  * @}
  */

/** @defgroup PWR_Attributes PWR Attributes
  * @brief PWR Privilege/NPrivilege and Secure/NSecure Attributes
  * @{
  */
/* Defines attribute */
#define PWR_ITEM_ATTR_NSEC_PRIV_MASK   (0x10U)                                 /*!< NSecure Privilege / NPrivilege attribute item mask */
#define PWR_ITEM_ATTR_SEC_PRIV_MASK    (0x20U)                                 /*!< Secure Privilege / NPrivilege attribute item mask  */

#define PWR_NSEC_PRIV                  (PWR_ITEM_ATTR_NSEC_PRIV_MASK | 0x01U)  /*!< NSecure and Privileged attribute  */
#define PWR_NSEC_NPRIV                 PWR_ITEM_ATTR_NSEC_PRIV_MASK            /*!< NSecure and NPrivileged attribute */
#define PWR_SEC_PRIV                   (PWR_ITEM_ATTR_SEC_PRIV_MASK  | 0x02U)  /*!< Secure and Privileged attribute   */
#define PWR_SEC_NPRIV                  PWR_ITEM_ATTR_SEC_PRIV_MASK             /*!< Secure and NPrivileged attribute  */
/**
  * @}
  */

/** @defgroup PWR_Flag PWR Status Flags
  *        Elements values convention: 0000 00XX XXXX XXXY YYYYb
  *           -      Y YYYY : Flag position in the XXX register (5 bits)
  *           - XX XXXX XXX : Status register (9 bits)
  *             - 000000001: CR2    register
  *             - 000000010: VOSCR  register
  *             - 000000100: CR3    register
  *             - 000001000: BDCR1  register
  *             - 000010000: CPUCR  register
  *             - 000100000: SVMCR1 register
  *             - 001000000: SVMCR2 register
  *             - 010000000: SVMCR3 register
  *             - 100000000: WKUPSR register
  * @{
  */
/* CR2 */
#define PWR_FLAG_PVDO                  (0x0028U)                               /*!< Programmable voltage detect output */

/* VOSCR */
#define PWR_FLAG_ACTVOSRDY             (0x0051U)                               /*!< Voltage levels ready bit                                 */
#define PWR_FLAG_ACTVOS                (0x0050U)                               /*!< Programmable voltage detect output                       */
#define PWR_FLAG_VOSRDY                (0x0041U)                               /*!< VOS Ready bit for VCORE voltage scaling output selection */

/* CR3 */
#define PWR_FLAG_VCOREH                (0x0089U)                               /*!< VDDCORE level monitoring versus high threshold */
#define PWR_FLAG_VCOREL                (0x0088U)                               /*!< VDDCORE level monitoring versus low threshold  */

/* BDCR1 */
#define PWR_FLAG_TEMPH                 (0x0113U)                               /*!< Temperature level monitoring versus high threshold */
#define PWR_FLAG_TEMPL                 (0x0112U)                               /*!< Temperature level monitoring versus low threshold  */
#define PWR_FLAG_VBATH                 (0x0111U)                               /*!< VBAT level monitoring versus high threshold        */
#define PWR_FLAG_VBATL                 (0x0110U)                               /*!< VBAT level monitoring versus low threshold         */

/* CPUCR */
#define PWR_FLAG_SBF                   (0x0209U)                               /*!< System Standby flag */
#define PWR_FLAG_STOPF                 (0x0208U)                               /*!< System Stop flag    */

/* SVMCR1 */
#define PWR_FLAG_VDDIO4RDY             (0x0410U)                               /*!< VDDIO4 ready  */


/* SVMCR2 */
#define PWR_FLAG_VDDIO5RDY             (0x0810U)                               /*!< VDDIO5 ready  */

/* SVMCR3 */
#define PWR_FLAG_ARDY                  (0x1014U)                               /*!< VDDA18ADC ready */
#define PWR_FLAG_USB33RDY              (0x1012U)                               /*!< VDD33USB ready  */
#define PWR_FLAG_VDDIO3RDY             (0x1011U)                               /*!< VDDIO3 ready    */
#define PWR_FLAG_VDDIO2RDY             (0x1010U)                               /*!< VDDIO2 ready    */

/* WKUPSR */
#define PWR_FLAG_WKUPF4                (0x2004U)                               /*!< Wakeup event on wakeup pin 4 */
#define PWR_FLAG_WKUPF3                (0x2003U)                               /*!< Wakeup event on wakeup pin 3 */
#define PWR_FLAG_WKUPF2                (0x2002U)                               /*!< Wakeup event on wakeup pin 2 */
#define PWR_FLAG_WKUPF1                (0x2001U)                               /*!< Wakeup event on wakeup pin 1 */
/**
  * @}
  */

/**
  * @}
  */
/* Exported macro ------------------------------------------------------------*/
/** @defgroup PWR_Exported_Macro PWR Exported Macro
  * @{
  */

/** @brief  Check whether or not a specific PWR flag is set.
  * @param  __FLAG__ specifies the flag to check.
  *           This parameter can be one of the following values:
  *          @arg PWR_FLAG_PVDO:      This flag is valid only if PVD
  *                                   is enabled by the HAL_PWR_EnablePVD()function.
  *                                   The PVD is stopped by STANDBY mode.
  *          @arg PWR_FLAG_ACTVOSRDY: This flag indicates that the regulator voltage
  *                                   scaling output selection is ready.
  *          @arg PWR_FLAG_ACTVOS:    This flag indicates that the regulator voltage
  *                                   scaling output selection is ready.
  *          @arg PWR_FLAG_VOSRDY     When an internal regulator is used, this bit indicates
  *                                   that all the features allowed by the selected VOS can be used.
  *          @arg PWR_FLAG_VCOREH:    This flag indicates if the VDDCORE level is above
  *                                   the high monitoring threshold.
  *          @arg PWR_FLAG_VCOREL     This flag indicates if the VDDCORE level is above
  *                                   the low monitoring threshold.
  *          @arg PWR_FLAG_TEMPH:     This flag indicates if the temperature level is above
  *                                   the high monitoring threshold.
  *          @arg PWR_FLAG_TEMPL      This flag indicates if the temperature level is above
  *                                   the low monitoring threshold.
  *          @arg PWR_FLAG_VBATH:     This flag indicates if the VBAT level is above
  *                                   the high monitoring threshold.
  *          @arg PWR_FLAG_VBATL:     This flag indicates if the VBAT level is above
  *                                   the low monitoring threshold.
  *          @arg PWR_FLAG_SBF:       Standby mode flag.
  *          @arg PWR_FLAG_STOPF:     STOP mode flag.
  *          @arg PWR_FLAG_VDDIO5RDY: This flag indicates if the VDDIO5 is ready.
  *          @arg PWR_FLAG_VDDIO4RDY: This flag indicates if the VDDIO4 is ready.
  *          @arg PWR_FLAG_VDDIO3RDY: This flag indicates if the VDDIO3 is ready.
  *          @arg PWR_FLAG_VDDIO2RDY: This flag indicates if the VDDIO2 is ready.
  *          @arg PWR_FLAG_ARDY:      This flag indicates if the VDDA18ADC is ready.
  *          @arg PWR_FLAG_USB33RDY:  This flag indicates if the VDD33USB is ready.
  *          @arg PWR_FLAG_WUF1:      This flag indicates a wakeup event was
  *                                   received from WKUP1 pin.
  *          @arg PWR_FLAG_WUF2:      This flag indicates a wakeup event was
  *                                   received from WKUP2 pin.
  *          @arg PWR_FLAG_WUF3:      This flag indicates a wakeup event was
  *                                   received from WKUP3 pin.
  *          @arg PWR_FLAG_WUF4:      This flag indicates a wakeup event was
  *                                   received from WKUP4 pin.
  * @retval The (__FLAG__) state (TRUE or FALSE).
  */
#define __HAL_PWR_GET_FLAG(__FLAG__) ( \
                ((__FLAG__) == PWR_FLAG_PVDO)      ? ((PWR->CR2 & PWR_CR2_PVDO)            == PWR_CR2_PVDO)         : \
                ((__FLAG__) == PWR_FLAG_ACTVOSRDY) ? ((PWR->VOSCR & PWR_VOSCR_ACTVOSRDY)   == PWR_VOSCR_ACTVOSRDY)  : \
                ((__FLAG__) == PWR_FLAG_ACTVOS)    ? ((PWR->VOSCR & PWR_VOSCR_ACTVOS)      == PWR_VOSCR_ACTVOS)     : \
                ((__FLAG__) == PWR_FLAG_VOSRDY)    ? ((PWR->VOSCR & PWR_VOSCR_VOSRDY)      == PWR_VOSCR_VOSRDY)     : \
                ((__FLAG__) == PWR_FLAG_VCOREH)    ? ((PWR->CR3 & PWR_CR3_VCOREH)          == PWR_CR3_VCOREH)       : \
                ((__FLAG__) == PWR_FLAG_VCOREL)    ? ((PWR->CR3 & PWR_CR3_VCOREL)          == PWR_CR3_VCOREL)       : \
                ((__FLAG__) == PWR_FLAG_TEMPH)     ? ((PWR->BDCR1 & PWR_BDCR1_TEMPH)       == PWR_BDCR1_TEMPH)      : \
                ((__FLAG__) == PWR_FLAG_TEMPL)     ? ((PWR->BDCR1 & PWR_BDCR1_TEMPL)       == PWR_BDCR1_TEMPL)      : \
                ((__FLAG__) == PWR_FLAG_VBATH)     ? ((PWR->BDCR1 & PWR_BDCR1_VBATH)       == PWR_BDCR1_VBATH)      : \
                ((__FLAG__) == PWR_FLAG_VBATL)     ? ((PWR->BDCR1 & PWR_BDCR1_VBATL)       == PWR_BDCR1_VBATL)      : \
                ((__FLAG__) == PWR_FLAG_SBF)       ? ((PWR->CPUCR & PWR_CPUCR_SBF)         == PWR_CPUCR_SBF)        : \
                ((__FLAG__) == PWR_FLAG_STOPF)     ? ((PWR->CPUCR & PWR_CPUCR_STOPF)       == PWR_CPUCR_STOPF)      : \
                ((__FLAG__) == PWR_FLAG_VDDIO4RDY) ? ((PWR->SVMCR1 & PWR_SVMCR1_VDDIO4RDY) == PWR_SVMCR1_VDDIO4RDY) : \
                ((__FLAG__) == PWR_FLAG_VDDIO5RDY) ? ((PWR->SVMCR2 & PWR_SVMCR2_VDDIO5RDY) == PWR_SVMCR2_VDDIO5RDY) : \
                ((__FLAG__) == PWR_FLAG_ARDY)      ? ((PWR->SVMCR3 & PWR_SVMCR3_ARDY)      == PWR_SVMCR3_ARDY)      : \
                ((__FLAG__) == PWR_FLAG_USB33RDY)  ? ((PWR->SVMCR3 & PWR_SVMCR3_USB33RDY)  == PWR_SVMCR3_USB33RDY)  : \
                ((__FLAG__) == PWR_FLAG_VDDIO3RDY) ? ((PWR->SVMCR3 & PWR_SVMCR3_VDDIO3RDY) == PWR_SVMCR3_VDDIO3RDY) : \
                ((__FLAG__) == PWR_FLAG_VDDIO2RDY) ? ((PWR->SVMCR3 & PWR_SVMCR3_VDDIO2RDY) == PWR_SVMCR3_VDDIO2RDY) : \
                ((__FLAG__) == PWR_FLAG_WKUPF4)    ? ((PWR->WKUPSR & PWR_WKUPSR_WKUPF4)    == PWR_WKUPSR_WKUPF4)    : \
                ((__FLAG__) == PWR_FLAG_WKUPF3)    ? ((PWR->WKUPSR & PWR_WKUPSR_WKUPF3)    == PWR_WKUPSR_WKUPF3)    : \
                ((__FLAG__) == PWR_FLAG_WKUPF2)    ? ((PWR->WKUPSR & PWR_WKUPSR_WKUPF2)    == PWR_WKUPSR_WKUPF2)    : \
                ((__FLAG__) == PWR_FLAG_WKUPF1)    ? ((PWR->WKUPSR & PWR_WKUPSR_WKUPF1)    == PWR_WKUPSR_WKUPF1)    : \
                ((PWR->WKUPSR & PWR_WKUPSR_WKUPF4) == PWR_WKUPSR_WKUPF4))

/** @brief  Clear PWR flags.
  * @param  __FLAG__: specifies the flag to clear.
  *         This parameter can be one of the following values:
  *           @arg PWR_FLAG_STOPF  : Stop flag.
  *                                  Indicates that the device was resumed from Stop mode.
  *           @arg PWR_FLAG_SBF    : Standby flag.
  *                                  Indicates that the device was resumed from Standby mode.
  * @retval None.
  */
#define __HAL_PWR_CLEAR_FLAG(__FLAG__) ( \
                                        ((__FLAG__) == PWR_FLAG_STOPF)  ? (SET_BIT(PWR->CPUCR,   PWR_CPUCR_CSSF))   : \
                                        ((__FLAG__) == PWR_FLAG_SBF)    ? (SET_BIT(PWR->CPUCR,   PWR_CPUCR_CSSF))   : \
                                        (SET_BIT(PWR->CPUCR, PWR_CPUCR_CSSF)))

/** @brief  Check PWR wake up flags are set or not.
  * @param  __FLAG__: specifies the wake up flag to check.
  *           This parameter can be one of the following values:
  *            @arg PWR_FLAG_WKUP1 : This parameter gets Wake up line 1 flag.
  *            @arg PWR_FLAG_WKUP2 : This parameter gets Wake up line 2 flag.
  *            @arg PWR_FLAG_WKUP3 : This parameter gets Wake up line 3 flag.
  *            @arg PWR_FLAG_WKUP4 : This parameter gets Wake up line 4 flag.
  *            @arg PWR_FLAG_WKUP  : This parameter gets Wake up lines 1 to 4 flags.
  * @retval The (__FLAG__) state (TRUE or FALSE).
  */
#define __HAL_PWR_GET_WAKEUPFLAG(__FLAG__) ((PWR->WKUPFR & (__FLAG__)) ? 0U : 1U)

/** @brief  Clear CPU PWR wake up flags.
  * @param  __FLAG__ : Specifies the wake up flag to be cleared.
  *           This parameter can be one of the following values :
  *            @arg PWR_FLAG_WKUP1 : This parameter clears Wake up line 1 flag.
  *            @arg PWR_FLAG_WKUP2 : This parameter clears Wake up line 2 flag.
  *            @arg PWR_FLAG_WKUP3 : This parameter clears Wake up line 3 flag.
  *            @arg PWR_FLAG_WKUP4 : This parameter clears Wake up line 4 flag.
  *            @arg PWR_FLAG_WKUP  : This parameter clears Wake up lines 1 to 4 flags.
  * @retval None.
  */
#define __HAL_PWR_CLEAR_WAKEUPFLAG(__FLAG__) SET_BIT(PWR->WKUPCR, (__FLAG__))

/**
  * @brief  Enable the PVD Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_IT() SET_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVD)

/**
  * @brief  Disable the PVD Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_IT() CLEAR_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVD)

/**
  * @brief  Enable the PVD Event Line.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_EVENT() SET_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVD)

/**
  * @brief  Disable the PVD Event Line.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_EVENT() CLEAR_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVD)

/**
  * @brief  Enable the PVD Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_RISING_EDGE() SET_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVD)

/**
  * @brief  Disable the PVD Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_RISING_EDGE() CLEAR_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVD)

/**
  * @brief  Enable the PVD Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_FALLING_EDGE() SET_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVD)

/**
  * @brief  Disable the PVD Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_FALLING_EDGE() CLEAR_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVD)

/**
  * @brief  Enable the PVD Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_RISING_FALLING_EDGE() \
  do                                                    \
  {                                                     \
    __HAL_PWR_PVD_EXTI_ENABLE_RISING_EDGE();            \
    __HAL_PWR_PVD_EXTI_ENABLE_FALLING_EDGE();           \
  } while(0);

/**
  * @brief  Disable the PVD Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_RISING_FALLING_EDGE() \
  do                                                     \
  {                                                      \
    __HAL_PWR_PVD_EXTI_DISABLE_RISING_EDGE();            \
    __HAL_PWR_PVD_EXTI_DISABLE_FALLING_EDGE();           \
  } while(0);

/**
  * @brief  Check whether or not the PVD EXTI interrupt Rising flag is set.
  * @retval EXTI PVD Line Status.
  */
#define __HAL_PWR_PVD_EXTI_GET_RISING_FLAG() (EXTI->RPR3 & PWR_EXTI_LINE_PVD)

/**
  * @brief  Check whether or not the PVD EXTI interrupt Falling flag is set.
  * @retval EXTI PVD Line Status.
  */
#define __HAL_PWR_PVD_EXTI_GET_FALLING_FLAG() (EXTI->FPR3 & PWR_EXTI_LINE_PVD)

/**
  * @brief  Clear the PVD EXTI interrupt Rising flag.
  * @retval None
  */
#define __HAL_PWR_PVD_EXTI_CLEAR_RISING_FLAG() WRITE_REG(EXTI->RPR3, PWR_EXTI_LINE_PVD)

/**
  * @brief  Clear the PVD EXTI interrupt Falling flag.
  * @retval None
  */
#define __HAL_PWR_PVD_EXTI_CLEAR_FALLING_FLAG() WRITE_REG(EXTI->FPR3, PWR_EXTI_LINE_PVD)

/**
  * @brief  Generates a Software interrupt on PVD EXTI line.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER3, PWR_EXTI_LINE_PVD)
/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup PWR_Private_Constants PWR Private Constants
  * @{
  */

/** @defgroup PWR_PVD_EXTI_Line PWR PVD EXTI Line
  * @{
  */
#define PWR_EXTI_LINE_PVD              EXTI_IMR3_IM66                          /*!< External interrupt line 66 connected to the PVD EXTI Line */
/**
  * @}
  */

/**
  * @}
  */
/* Private macros ------------------------------------------------------------*/
/** @defgroup PWR_Private_Macros PWR Private Macros
  * @{
  */

/** @defgroup PWR_IS_PWR_Definitions PWR Private macros to check input parameters
  * @{
  */

/* Check PVD mode parameter */
#define IS_PWR_PVD_MODE(__MODE__) (((__MODE__) == PWR_PVD_MODE_IT_RISING)            ||\
                                   ((__MODE__) == PWR_PVD_MODE_IT_FALLING)           ||\
                                   ((__MODE__) == PWR_PVD_MODE_IT_RISING_FALLING)    ||\
                                   ((__MODE__) == PWR_PVD_MODE_EVENT_RISING)         ||\
                                   ((__MODE__) == PWR_PVD_MODE_EVENT_FALLING)        ||\
                                   ((__MODE__) == PWR_PVD_MODE_EVENT_RISING_FALLING) ||\
                                   ((__MODE__) == PWR_PVD_MODE_NORMAL))

/* Check Sleep mode entry parameter */
#define IS_PWR_SLEEP_ENTRY(__ENTRY__) (((__ENTRY__) == PWR_SLEEPENTRY_WFI) ||\
                                       ((__ENTRY__) == PWR_SLEEPENTRY_WFE) ||\
                                       ((__ENTRY__) == PWR_SLEEPENTRY_WFE_NO_EVT_CLEAR))

/* Check Stop mode mode entry parameter */
#define IS_PWR_STOP_ENTRY(__ENTRY__) (((__ENTRY__) == PWR_STOPENTRY_WFI) ||\
                                      ((__ENTRY__) == PWR_STOPENTRY_WFE) ||\
                                      ((__ENTRY__) == PWR_STOPENTRY_WFE_NO_EVT_CLEAR))

/* Check wake up pin parameter */
#define IS_PWR_WAKEUP_PIN(__PIN__) (((__PIN__) == PWR_WAKEUP_PIN1)      ||\
                                    ((__PIN__) == PWR_WAKEUP_PIN2)      ||\
                                    ((__PIN__) == PWR_WAKEUP_PIN3)      ||\
                                    ((__PIN__) == PWR_WAKEUP_PIN4)      ||\
                                    ((__PIN__) == PWR_WAKEUP_PIN1_HIGH) ||\
                                    ((__PIN__) == PWR_WAKEUP_PIN2_HIGH) ||\
                                    ((__PIN__) == PWR_WAKEUP_PIN3_HIGH) ||\
                                    ((__PIN__) == PWR_WAKEUP_PIN4_HIGH) ||\
                                    ((__PIN__) == PWR_WAKEUP_PIN1_LOW)  ||\
                                    ((__PIN__) == PWR_WAKEUP_PIN2_LOW)  ||\
                                    ((__PIN__) == PWR_WAKEUP_PIN3_LOW)  ||\
                                    ((__PIN__) == PWR_WAKEUP_PIN4_LOW))

/* Check wake up pin polarity parameter */
#define IS_PWR_WAKEUP_PIN_POLARITY(__POLARITY__) (((__POLARITY__) == PWR_PIN_POLARITY_HIGH) ||\
                                                  ((__POLARITY__) == PWR_PIN_POLARITY_LOW))

/* Check wake up pin pull configuration parameter */
#define IS_PWR_WAKEUP_PIN_PULL(__PULL__) (((__PULL__) == PWR_PIN_NO_PULL) ||\
                                          ((__PULL__) == PWR_PIN_PULL_UP) ||\
                                          ((__PULL__) == PWR_PIN_PULL_DOWN))

/* Check wake up flag parameter */
#define IS_PWR_WAKEUP_FLAG(__FLAG__) (((__FLAG__) == PWR_WAKEUP_FLAG1) ||\
                                      ((__FLAG__) == PWR_WAKEUP_FLAG2) ||\
                                      ((__FLAG__) == PWR_WAKEUP_FLAG3) ||\
                                      ((__FLAG__) == PWR_WAKEUP_FLAG4) ||\
                                      ((__FLAG__) == PWR_WAKEUP_FLAG_ALL))


/* PWR Secure/Privilege items check macro */
#define IS_PWR_ITEMS_ATTRIBUTES(__ITEM__) ((((__ITEM__) & (~PWR_ITEM_ALL)) == 0U) && ((__ITEM__) != 0U))

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
/* PWR attribute check macro (Secure) */
#define IS_PWR_ATTRIBUTES(ATTRIBUTES) ((((~(((ATTRIBUTES)& \
                                             0xF0U) >> 4U)) & ((ATTRIBUTES) & 0x0FU)) == 0U) && \
                                       (((ATTRIBUTES) & 0xFFFFFFCCU) == 0U))
#else
/* PWR attribute check macro (NSecure) */
#define IS_PWR_ATTRIBUTES(ATTRIBUTES) (((ATTRIBUTES) == PWR_NSEC_NPRIV) || ((ATTRIBUTES) == PWR_NSEC_PRIV))
#endif /* __ARM_FEATURE_CMSE */

/**
  * @}
  */

/**
  * @}
  */

/* Include PWR HAL Extended module */
#include "stm32n6xx_hal_pwr_ex.h"

/* Exported functions --------------------------------------------------------*/

/** @addtogroup PWR_Exported_Functions PWR Exported Functions
  * @{
  */

/** @addtogroup PWR_Exported_Functions_Group1 Initialization and De-Initialization Functions
  * @{
  */
/* Initialization and de-initialization functions *****************************/
void HAL_PWR_DeInit(void);
void HAL_PWR_EnableBkUpAccess(void);
void HAL_PWR_DisableBkUpAccess(void);
/**
  * @}
  */

/** @addtogroup PWR_Exported_Functions_Group2 Peripheral Control Functions
  * @{
  */
/* Peripheral Control Functions  **********************************************/
/* PVD configuration */
void HAL_PWR_ConfigPVD(const PWR_PVDTypeDef *pConfigPVD);
void HAL_PWR_EnablePVD(void);
void HAL_PWR_DisablePVD(void);

/* WakeUp pins configuration */
void              HAL_PWR_EnableWakeUpPin(uint32_t WakeUpPinPolarity);
void              HAL_PWR_DisableWakeUpPin(uint32_t WakeUpPinx);
uint32_t          HAL_PWR_GetWakeupFlag(uint32_t WakeUpFlag);
HAL_StatusTypeDef HAL_PWR_ClearWakeupFlag(uint32_t WakeUpFlag);

/* Low Power modes entry */
void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry);
void HAL_PWR_EnterSLEEPMode(uint32_t Regulator, uint8_t SLEEPEntry);
void HAL_PWR_EnterSTANDBYMode(void);

/* Cortex System Control functions  *******************************************/
void HAL_PWR_EnableSleepOnExit(void);
void HAL_PWR_DisableSleepOnExit(void);
void HAL_PWR_EnableSEVOnPend(void);
void HAL_PWR_DisableSEVOnPend(void);

/* Interrupt handler functions ************************************************/
void HAL_PWR_PVD_Rising_Callback(void);
void HAL_PWR_PVD_Falling_Callback(void);
void HAL_PWR_WAKEUP_PIN_IRQHandler(void);
void HAL_PWR_WKUP1_Callback(void);
void HAL_PWR_WKUP2_Callback(void);
void HAL_PWR_WKUP3_Callback(void);
void HAL_PWR_WKUP4_Callback(void);
/**
  * @}
  */

/** @addtogroup PWR_Exported_Functions_Group3
  * @{
  */
/* Privileges and security configuration functions ****************************/
void              HAL_PWR_ConfigAttributes(uint32_t Item, uint32_t Attributes);
HAL_StatusTypeDef HAL_PWR_GetConfigAttributes(uint32_t Item, uint32_t *pAttributes);
/**
  * @}
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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STM32N6xx_HAL_PWR_H */

