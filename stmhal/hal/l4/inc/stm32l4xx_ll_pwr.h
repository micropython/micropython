/**
  ******************************************************************************
  * @file    stm32l4xx_ll_pwr.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of PWR LL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#ifndef __STM32L4xx_LL_PWR_H
#define __STM32L4xx_LL_PWR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

#if defined(PWR)

/** @defgroup PWR_LL PWR
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup PWR_LL_Exported_Constants PWR Exported Constants
  * @{
  */

/** @defgroup PWR_LL_EC_CLEAR_FLAG Clear Flags Defines
  * @brief    Flags defines which can be used with LL_PWR_WriteReg function
  * @{
  */
#define LL_PWR_SCR_CSBF                    PWR_SCR_CSBF
#define LL_PWR_SCR_CWUF                    PWR_SCR_CWUF
#define LL_PWR_SCR_CWUF5                   PWR_SCR_CWUF5
#define LL_PWR_SCR_CWUF4                   PWR_SCR_CWUF4
#define LL_PWR_SCR_CWUF3                   PWR_SCR_CWUF3
#define LL_PWR_SCR_CWUF2                   PWR_SCR_CWUF2
#define LL_PWR_SCR_CWUF1                   PWR_SCR_CWUF1
/**
  * @}
  */

/** @defgroup PWR_LL_EC_GET_FLAG Get Flags Defines
  * @brief    Flags defines which can be used with LL_PWR_ReadReg function
  * @{
  */
#define LL_PWR_SR1_WUFI                    PWR_SR1_WUFI
#define LL_PWR_SR1_SBF                     PWR_SR1_SBF
#define LL_PWR_SR1_WUF5                    PWR_SR1_WUF5
#define LL_PWR_SR1_WUF4                    PWR_SR1_WUF4
#define LL_PWR_SR1_WUF3                    PWR_SR1_WUF3
#define LL_PWR_SR1_WUF2                    PWR_SR1_WUF2
#define LL_PWR_SR1_WUF1                    PWR_SR1_WUF1
#define LL_PWR_SR2_PVMO4                   PWR_SR2_PVMO4
#define LL_PWR_SR2_PVMO3                   PWR_SR2_PVMO3
#if defined(PWR_SR2_PVMO2)
#define LL_PWR_SR2_PVMO2                   PWR_SR2_PVMO2
#endif /* PWR_SR2_PVMO2 */
#define LL_PWR_SR2_PVMO1                   PWR_SR2_PVMO1
#define LL_PWR_SR2_PVDO                    PWR_SR2_PVDO
#define LL_PWR_SR2_VOSF                    PWR_SR2_VOSF
#define LL_PWR_SR2_REGLPF                  PWR_SR2_REGLPF
#define LL_PWR_SR2_REGLPS                  PWR_SR2_REGLPS
/**
  * @}
  */

/** @defgroup PWR_LL_EC_REGU_VOLTAGE REGU VOLTAGE
  * @{
  */
#define LL_PWR_REGU_VOLTAGE_SCALE1         (PWR_CR1_VOS_0)
#define LL_PWR_REGU_VOLTAGE_SCALE2         (PWR_CR1_VOS_1)
/**
  * @}
  */

/** @defgroup PWR_LL_EC_MODE_PWR MODE PWR
  * @{
  */
#define LL_PWR_MODE_STOP0                  (PWR_CR1_LPMS_STOP0)
#define LL_PWR_MODE_STOP1                  (PWR_CR1_LPMS_STOP1)
#define LL_PWR_MODE_STOP2                  (PWR_CR1_LPMS_STOP2)
#define LL_PWR_MODE_STANDBY                (PWR_CR1_LPMS_STANDBY)
#define LL_PWR_MODE_SHUTDOWN               (PWR_CR1_LPMS_SHUTDOWN)
/**
  * @}
  */

/** @defgroup PWR_LL_EC_PVM_VDDUSB_1 PVM VDDUSB 1
  * @{
  */
#if defined(USB_OTG_FS)
#define LL_PWR_PVM_VDDUSB_1_2V             (PWR_CR2_PVME1)     /* Monitoring VDDUSB vs. 1.2V */
#endif /* USB_OTG_FS */
#if defined(PWR_CR2_PVME2)
#define LL_PWR_PVM_VDDIO2_0_9V             (PWR_CR2_PVME2)     /* Monitoring VDDIO2 vs. 0.9V */
#endif /* PWR_CR2_PVME2 */
#define LL_PWR_PVM_VDDA_1_62V              (PWR_CR2_PVME3)     /* Monitoring VDDA vs. 1.62V  */
#define LL_PWR_PVM_VDDA_2_2V               (PWR_CR2_PVME4)     /* Monitoring VDDA vs. 2.2V   */
/**
  * @}
  */

/** @defgroup PWR_LL_EC_PVDLEVEL PVDLEVEL
  * @{
  */
#define LL_PWR_PVDLEVEL_0                  (PWR_CR2_PLS_LEV0)  /* VPVD0 around 2.0 V */
#define LL_PWR_PVDLEVEL_1                  (PWR_CR2_PLS_LEV1)  /* VPVD1 around 2.2 V */
#define LL_PWR_PVDLEVEL_2                  (PWR_CR2_PLS_LEV2)  /* VPVD2 around 2.4 V */
#define LL_PWR_PVDLEVEL_3                  (PWR_CR2_PLS_LEV3)  /* VPVD3 around 2.5 V */
#define LL_PWR_PVDLEVEL_4                  (PWR_CR2_PLS_LEV4)  /* VPVD4 around 2.6 V */
#define LL_PWR_PVDLEVEL_5                  (PWR_CR2_PLS_LEV5)  /* VPVD5 around 2.8 V */
#define LL_PWR_PVDLEVEL_6                  (PWR_CR2_PLS_LEV6)  /* VPVD6 around 2.9 V */
#define LL_PWR_PVDLEVEL_7                  (PWR_CR2_PLS_LEV7)  /* External input analog voltage   (Compare internally to VREFINT) */
/**
  * @}
  */

/** @defgroup PWR_LL_EC_WAKEUP WAKEUP
  * @{
  */
#define LL_PWR_WAKEUP_PIN1                 (PWR_CR3_EWUP1)
#define LL_PWR_WAKEUP_PIN2                 (PWR_CR3_EWUP2)
#define LL_PWR_WAKEUP_PIN3                 (PWR_CR3_EWUP3)
#define LL_PWR_WAKEUP_PIN4                 (PWR_CR3_EWUP4)
#define LL_PWR_WAKEUP_PIN5                 (PWR_CR3_EWUP5)
/**
  * @}
  */

/** @defgroup PWR_LL_EC_BATT_CHARG_RESISTOR BATT CHARG RESISTOR
  * @{
  */
#define LL_PWR_BATT_CHARG_RESISTOR_5K      ((uint32_t)0x00000000)
#define LL_PWR_BATT_CHARGRESISTOR_1_5K     (PWR_CR4_VBRS)
/**
  * @}
  */

/** @defgroup PWR_LL_EC_GPIO GPIO
  * @{
  */
#define LL_PWR_GPIO_A                      ((uint32_t)(&(PWR->PUCRA)))
#define LL_PWR_GPIO_B                      ((uint32_t)(&(PWR->PUCRB)))
#define LL_PWR_GPIO_C                      ((uint32_t)(&(PWR->PUCRC)))
#define LL_PWR_GPIO_D                      ((uint32_t)(&(PWR->PUCRD)))
#define LL_PWR_GPIO_E                      ((uint32_t)(&(PWR->PUCRE)))
#if defined(GPIOF)
#define LL_PWR_GPIO_F                      ((uint32_t)(&(PWR->PUCRF)))
#endif /* GPIOF */
#if defined(GPIOG)
#define LL_PWR_GPIO_G                      ((uint32_t)(&(PWR->PUCRG)))
#endif /* GPIOG */
#define LL_PWR_GPIO_H                      ((uint32_t)(&(PWR->PUCRH)))
/**
  * @}
  */

/** @defgroup PWR_LL_EC_GPIO_BIT GPIO BIT
  * @{
  */
#define LL_PWR_GPIO_BIT_0                  ((uint32_t)0x00000001)
#define LL_PWR_GPIO_BIT_1                  ((uint32_t)0x00000002)
#define LL_PWR_GPIO_BIT_2                  ((uint32_t)0x00000004)
#define LL_PWR_GPIO_BIT_3                  ((uint32_t)0x00000008)
#define LL_PWR_GPIO_BIT_4                  ((uint32_t)0x00000010)
#define LL_PWR_GPIO_BIT_5                  ((uint32_t)0x00000020)
#define LL_PWR_GPIO_BIT_6                  ((uint32_t)0x00000040)
#define LL_PWR_GPIO_BIT_7                  ((uint32_t)0x00000080)
#define LL_PWR_GPIO_BIT_8                  ((uint32_t)0x00000100)
#define LL_PWR_GPIO_BIT_9                  ((uint32_t)0x00000200)
#define LL_PWR_GPIO_BIT_10                 ((uint32_t)0x00000400)
#define LL_PWR_GPIO_BIT_11                 ((uint32_t)0x00000800)
#define LL_PWR_GPIO_BIT_12                 ((uint32_t)0x00001000)
#define LL_PWR_GPIO_BIT_13                 ((uint32_t)0x00002000)
#define LL_PWR_GPIO_BIT_14                 ((uint32_t)0x00004000)
#define LL_PWR_GPIO_BIT_15                 ((uint32_t)0x00008000)
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup PWR_LL_Exported_Macros PWR Exported Macros
  * @{
  */

/** @defgroup PWR_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in PWR register
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_PWR_WriteReg(__REG__, __VALUE__) WRITE_REG(PWR->__REG__, (__VALUE__))

/**
  * @brief  Read a value in PWR register
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_PWR_ReadReg(__REG__) READ_REG(PWR->__REG__)
/**
  * @}
  */

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @defgroup PWR_LL_Exported_Functions PWR Exported Functions
  * @{
  */

/** @defgroup PWR_LL_EF_Configuration Configuration
  * @{
  */

/**
  * @brief  Switch the regulator from main mode to low-power mode
  * @rmtoll CR1          LPR           LL_PWR_EnableLowPowerRunMode
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableLowPowerRunMode(void)
{
  SET_BIT(PWR->CR1, PWR_CR1_LPR);
}

/**
  * @brief  Switch the regulator from low-power mode to main mode
  * @rmtoll CR1          LPR           LL_PWR_DisableLowPowerRunMode
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableLowPowerRunMode(void)
{
  CLEAR_BIT(PWR->CR1, PWR_CR1_LPR);
}

/**
  * @brief  Check if the regulator is in low-power mode
  * @rmtoll CR1          LPR           LL_PWR_IsEnabledLowPowerRunMode
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledLowPowerRunMode(void)
{
  return (READ_BIT(PWR->CR1, PWR_CR1_LPR) == (PWR_CR1_LPR));
}

/**
  * @brief  Set the main internal regulator output voltage
  * @rmtoll CR1          VOS           LL_PWR_SetRegulVoltageScaling
  * @param  VoltageScaling This parameter can be one of the following values:
  *         @arg @ref LL_PWR_REGU_VOLTAGE_SCALE1
  *         @arg @ref LL_PWR_REGU_VOLTAGE_SCALE2
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetRegulVoltageScaling(uint32_t VoltageScaling)
{
  MODIFY_REG(PWR->CR1, PWR_CR1_VOS, VoltageScaling);
}

/**
  * @brief  Get the main internal regulator output voltage
  * @rmtoll CR1          VOS           LL_PWR_GetRegulVoltageScaling
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_REGU_VOLTAGE_SCALE1
  *         @arg @ref LL_PWR_REGU_VOLTAGE_SCALE2
  */
__STATIC_INLINE uint32_t LL_PWR_GetRegulVoltageScaling(void)
{
  return (uint32_t)(READ_BIT(PWR->CR1, PWR_CR1_VOS));
}

/**
  * @brief  Enable access to the backup domain
  * @rmtoll CR1          DBP           LL_PWR_EnableBkUpAccess
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableBkUpAccess(void)
{
  SET_BIT(PWR->CR1, PWR_CR1_DBP);
}

/**
  * @brief  Disable access to the backup domain
  * @rmtoll CR1          DBP           LL_PWR_DisableBkUpAccess
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableBkUpAccess(void)
{
  CLEAR_BIT(PWR->CR1, PWR_CR1_DBP);
}

/**
  * @brief  Check if the backup domain is enabled
  * @rmtoll CR1          DBP           LL_PWR_IsEnabledBkUpAccess
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledBkUpAccess(void)
{
  return (READ_BIT(PWR->CR1, PWR_CR1_DBP) == (PWR_CR1_DBP));
}

/**
  * @brief  Set Low-Power mode
  * @rmtoll CR1          LPMS          LL_PWR_SetPowerMode
  * @param  LowPowerMode This parameter can be one of the following values:
  *         @arg @ref LL_PWR_MODE_STOP0
  *         @arg @ref LL_PWR_MODE_STOP1
  *         @arg @ref LL_PWR_MODE_STOP2
  *         @arg @ref LL_PWR_MODE_STANDBY
  *         @arg @ref LL_PWR_MODE_SHUTDOWN
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetPowerMode(uint32_t LowPowerMode)
{
  MODIFY_REG(PWR->CR1, PWR_CR1_LPMS, LowPowerMode);
}

/**
  * @brief  Get Low-Power mode
  * @rmtoll CR1          LPMS          LL_PWR_GetPowerMode
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_MODE_STOP0
  *         @arg @ref LL_PWR_MODE_STOP1
  *         @arg @ref LL_PWR_MODE_STOP2
  *         @arg @ref LL_PWR_MODE_STANDBY
  *         @arg @ref LL_PWR_MODE_SHUTDOWN
  */
__STATIC_INLINE uint32_t LL_PWR_GetPowerMode(void)
{
  return (uint32_t)(READ_BIT(PWR->CR1, PWR_CR1_LPMS));
}

#if defined(USB_OTG_FS)
/**
  * @brief  Enable VDDUSB supply
  * @rmtoll CR2          USV           LL_PWR_EnableVddUSB
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddUSB(void)
{
  SET_BIT(PWR->CR2, PWR_CR2_USV);
}
#endif /* USB_OTG_FS */

#if defined(USB_OTG_FS)
/**
  * @brief  Disable VDDUSB supply
  * @rmtoll CR2          USV           LL_PWR_DisableVddUSB
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddUSB(void)
{
  CLEAR_BIT(PWR->CR2, PWR_CR2_USV);
}
#endif /* USB_OTG_FS */

#if defined(USB_OTG_FS)
/**
  * @brief  Check if VDDUSB supply is enabled
  * @rmtoll CR2          USV           LL_PWR_IsEnabledVddUSB
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddUSB(void)
{
  return (READ_BIT(PWR->CR2, PWR_CR2_USV) == (PWR_CR2_USV));
}
#endif /* USB_OTG_FS */

#if defined(PWR_CR2_IOSV)
/**
  * @brief  Enable VDDIO2 supply
  * @rmtoll CR2          IOSV          LL_PWR_EnableVddIO2
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddIO2(void)
{
  SET_BIT(PWR->CR2, PWR_CR2_IOSV);
}

/**
  * @brief  Disable VDDIO2 supply
  * @rmtoll CR2          IOSV          LL_PWR_DisableVddIO2
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddIO2(void)
{
  CLEAR_BIT(PWR->CR2, PWR_CR2_IOSV);
}

/**
  * @brief  Check if VDDIO2 supply is enabled
  * @rmtoll CR2          IOSV          LL_PWR_IsEnabledVddIO2
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddIO2(void)
{
  return (READ_BIT(PWR->CR2, PWR_CR2_IOSV) == (PWR_CR2_IOSV));
}
#endif /* PWR_CR2_IOSV */

/**
  * @brief  Enable the Power Voltage Monitoring on a peripheral
  * @rmtoll CR2          PVME1         LL_PWR_EnablePVM\n
  *         CR2          PVME2         LL_PWR_EnablePVM\n
  *         CR2          PVME3         LL_PWR_EnablePVM\n
  *         CR2          PVME4         LL_PWR_EnablePVM
  * @param  PeriphVoltage This parameter can be one of the following values:
  *         @arg @ref LL_PWR_PVM_VDDUSB_1_2V (*)
  *         @arg @ref LL_PWR_PVM_VDDIO2_0_9V (*)
  *         @arg @ref LL_PWR_PVM_VDDA_1_62V
  *         @arg @ref LL_PWR_PVM_VDDA_2_2V
  *
  *         (*) value not defined in all devices
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnablePVM(uint32_t PeriphVoltage)
{
  SET_BIT(PWR->CR2, PeriphVoltage);
}

/**
  * @brief  Disable the Power Voltage Monitoring on a peripheral
  * @rmtoll CR2          PVME1         LL_PWR_DisablePVM\n
  *         CR2          PVME2         LL_PWR_DisablePVM\n
  *         CR2          PVME3         LL_PWR_DisablePVM\n
  *         CR2          PVME4         LL_PWR_DisablePVM
  * @param  PeriphVoltage This parameter can be one of the following values:
  *         @arg @ref LL_PWR_PVM_VDDUSB_1_2V (*)
  *         @arg @ref LL_PWR_PVM_VDDIO2_0_9V (*)
  *         @arg @ref LL_PWR_PVM_VDDA_1_62V
  *         @arg @ref LL_PWR_PVM_VDDA_2_2V
  *
  *         (*) value not defined in all devices
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisablePVM(uint32_t PeriphVoltage)
{
  CLEAR_BIT(PWR->CR2, PeriphVoltage);
}

/**
  * @brief  Check if Power Voltage Monitoring  is enabled on a peripheral
  * @rmtoll CR2          PVME1         LL_PWR_IsEnabledPVM\n
  *         CR2          PVME2         LL_PWR_IsEnabledPVM\n
  *         CR2          PVME3         LL_PWR_IsEnabledPVM\n
  *         CR2          PVME4         LL_PWR_IsEnabledPVM
  * @param  PeriphVoltage This parameter can be one of the following values:
  *         @arg @ref LL_PWR_PVM_VDDUSB_1_2V (*)
  *         @arg @ref LL_PWR_PVM_VDDIO2_0_9V (*)
  *         @arg @ref LL_PWR_PVM_VDDA_1_62V
  *         @arg @ref LL_PWR_PVM_VDDA_2_2V
  *
  *         (*) value not defined in all devices
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledPVM(uint32_t PeriphVoltage)
{
  return (READ_BIT(PWR->CR2, PeriphVoltage) == (PeriphVoltage));
}

/**
  * @brief  Configure the voltage threshold detected by the Power Voltage Detector
  * @rmtoll CR2          PLS           LL_PWR_SetPVDLevel
  * @param  PVDLevel This parameter can be one of the following values:
  *         @arg @ref LL_PWR_PVDLEVEL_0
  *         @arg @ref LL_PWR_PVDLEVEL_1
  *         @arg @ref LL_PWR_PVDLEVEL_2
  *         @arg @ref LL_PWR_PVDLEVEL_3
  *         @arg @ref LL_PWR_PVDLEVEL_4
  *         @arg @ref LL_PWR_PVDLEVEL_5
  *         @arg @ref LL_PWR_PVDLEVEL_6
  *         @arg @ref LL_PWR_PVDLEVEL_7
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetPVDLevel(uint32_t PVDLevel)
{
  MODIFY_REG(PWR->CR2, PWR_CR2_PLS, PVDLevel);
}

/**
  * @brief  Get the voltage threshold detection
  * @rmtoll CR2          PLS           LL_PWR_GetPVDLevel
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_PVDLEVEL_0
  *         @arg @ref LL_PWR_PVDLEVEL_1
  *         @arg @ref LL_PWR_PVDLEVEL_2
  *         @arg @ref LL_PWR_PVDLEVEL_3
  *         @arg @ref LL_PWR_PVDLEVEL_4
  *         @arg @ref LL_PWR_PVDLEVEL_5
  *         @arg @ref LL_PWR_PVDLEVEL_6
  *         @arg @ref LL_PWR_PVDLEVEL_7
  */
__STATIC_INLINE uint32_t LL_PWR_GetPVDLevel(void)
{
  return (uint32_t)(READ_BIT(PWR->CR2, PWR_CR2_PLS));
}

/**
  * @brief  Enable Power Voltage Detector
  * @rmtoll CR2          PVDE          LL_PWR_EnablePVD
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnablePVD(void)
{
  SET_BIT(PWR->CR2, PWR_CR2_PVDE);
}

/**
  * @brief  Disable Power Voltage Detector
  * @rmtoll CR2          PVDE          LL_PWR_DisablePVD
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisablePVD(void)
{
  CLEAR_BIT(PWR->CR2, PWR_CR2_PVDE);
}

/**
  * @brief  Check if Power Voltage Detector is enabled
  * @rmtoll CR2          PVDE          LL_PWR_IsEnabledPVD
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledPVD(void)
{
  return (READ_BIT(PWR->CR2, PWR_CR2_PVDE) == (PWR_CR2_PVDE));
}

/**
  * @brief  Enable Internal Wake-up line
  * @rmtoll CR3          EIWF          LL_PWR_EnableInternWU
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableInternWU(void)
{
  SET_BIT(PWR->CR3, PWR_CR3_EIWF);
}

/**
  * @brief  Disable Internal Wake-up line
  * @rmtoll CR3          EIWF          LL_PWR_DisableInternWU
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableInternWU(void)
{
  CLEAR_BIT(PWR->CR3, PWR_CR3_EIWF);
}

/**
  * @brief  Check if Internal Wake-up line is enabled
  * @rmtoll CR3          EIWF          LL_PWR_IsEnabledInternWU
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledInternWU(void)
{
  return (READ_BIT(PWR->CR3, PWR_CR3_EIWF) == (PWR_CR3_EIWF));
}

/**
  * @brief  Enable pull-up and pull-down configuration
  * @rmtoll CR3          APC           LL_PWR_EnablePUPDCfg
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnablePUPDCfg(void)
{
  SET_BIT(PWR->CR3, PWR_CR3_APC);
}

/**
  * @brief  Disable pull-up and pull-down configuration
  * @rmtoll CR3          APC           LL_PWR_DisablePUPDCfg
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisablePUPDCfg(void)
{
  CLEAR_BIT(PWR->CR3, PWR_CR3_APC);
}

/**
  * @brief  Check if pull-up and pull-down configuration  is enabled
  * @rmtoll CR3          APC           LL_PWR_IsEnabledPUPDCfg
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledPUPDCfg(void)
{
  return (READ_BIT(PWR->CR3, PWR_CR3_APC) == (PWR_CR3_APC));
}

/**
  * @brief  Enable SRAM2 content retention in Standby mode
  * @rmtoll CR3          RRS           LL_PWR_EnableSRAM2Retention
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableSRAM2Retention(void)
{
  SET_BIT(PWR->CR3, PWR_CR3_RRS);
}

/**
  * @brief  Disable SRAM2 content retention in Standby mode
  * @rmtoll CR3          RRS           LL_PWR_DisableSRAM2Retention
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableSRAM2Retention(void)
{
  CLEAR_BIT(PWR->CR3, PWR_CR3_RRS);
}

/**
  * @brief  Check if SRAM2 content retention in Standby mode  is enabled
  * @rmtoll CR3          RRS           LL_PWR_IsEnabledSRAM2Retention
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledSRAM2Retention(void)
{
  return (READ_BIT(PWR->CR3, PWR_CR3_RRS) == (PWR_CR3_RRS));
}

/**
  * @brief  Enable the WakeUp PINx functionality
  * @rmtoll CR3          EWUP1         LL_PWR_EnableWakeUpPin\n
  *         CR3          EWUP2         LL_PWR_EnableWakeUpPin\n
  *         CR3          EWUP3         LL_PWR_EnableWakeUpPin\n
  *         CR3          EWUP4         LL_PWR_EnableWakeUpPin\n
  *         CR3          EWUP5         LL_PWR_EnableWakeUpPin
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  *         @arg @ref LL_PWR_WAKEUP_PIN5
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableWakeUpPin(uint32_t WakeUpPin)
{
  SET_BIT(PWR->CR3, WakeUpPin);
}

/**
  * @brief  Disable the WakeUp PINx functionality
  * @rmtoll CR3          EWUP1         LL_PWR_DisableWakeUpPin\n
  *         CR3          EWUP2         LL_PWR_DisableWakeUpPin\n
  *         CR3          EWUP3         LL_PWR_DisableWakeUpPin\n
  *         CR3          EWUP4         LL_PWR_DisableWakeUpPin\n
  *         CR3          EWUP5         LL_PWR_DisableWakeUpPin
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  *         @arg @ref LL_PWR_WAKEUP_PIN5
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableWakeUpPin(uint32_t WakeUpPin)
{
  CLEAR_BIT(PWR->CR3, WakeUpPin);
}

/**
  * @brief  Check if the WakeUp PINx functionality is enabled
  * @rmtoll CR3          EWUP1         LL_PWR_IsEnabledWakeUpPin\n
  *         CR3          EWUP2         LL_PWR_IsEnabledWakeUpPin\n
  *         CR3          EWUP3         LL_PWR_IsEnabledWakeUpPin\n
  *         CR3          EWUP4         LL_PWR_IsEnabledWakeUpPin\n
  *         CR3          EWUP5         LL_PWR_IsEnabledWakeUpPin
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  *         @arg @ref LL_PWR_WAKEUP_PIN5
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledWakeUpPin(uint32_t WakeUpPin)
{
  return (READ_BIT(PWR->CR3, WakeUpPin) == (WakeUpPin));
}

/**
  * @brief  Set the resistor impedance
  * @rmtoll CR4          VBRS          LL_PWR_SetBattChargResistor
  * @param  Resistor This parameter can be one of the following values:
  *         @arg @ref LL_PWR_BATT_CHARG_RESISTOR_5K
  *         @arg @ref LL_PWR_BATT_CHARGRESISTOR_1_5K
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetBattChargResistor(uint32_t Resistor)
{
  MODIFY_REG(PWR->CR4, PWR_CR4_VBRS, Resistor);
}

/**
  * @brief  Get the resistor impedance
  * @rmtoll CR4          VBRS          LL_PWR_GetBattChargResistor
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_BATT_CHARG_RESISTOR_5K
  *         @arg @ref LL_PWR_BATT_CHARGRESISTOR_1_5K
  */
__STATIC_INLINE uint32_t LL_PWR_GetBattChargResistor(void)
{
  return (uint32_t)(READ_BIT(PWR->CR4, PWR_CR4_VBRS));
}

/**
  * @brief  Enable battery charging
  * @rmtoll CR4          VBE           LL_PWR_EnableBatteryCharging
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableBatteryCharging(void)
{
  SET_BIT(PWR->CR4, PWR_CR4_VBE);
}

/**
  * @brief  Disable battery charging
  * @rmtoll CR4          VBE           LL_PWR_DisableBatteryCharging
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableBatteryCharging(void)
{
  CLEAR_BIT(PWR->CR4, PWR_CR4_VBE);
}

/**
  * @brief  Check if battery charging is enabled
  * @rmtoll CR4          VBE           LL_PWR_IsEnabledBatteryCharging
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledBatteryCharging(void)
{
  return (READ_BIT(PWR->CR4, PWR_CR4_VBE) == (PWR_CR4_VBE));
}

/**
  * @brief  Set the Wake-Up pin polarity low for the event detection
  * @rmtoll CR4          WP1           LL_PWR_SetWakeUpPinPolarityLow\n
  *         CR4          WP2           LL_PWR_SetWakeUpPinPolarityLow\n
  *         CR4          WP3           LL_PWR_SetWakeUpPinPolarityLow\n
  *         CR4          WP4           LL_PWR_SetWakeUpPinPolarityLow\n
  *         CR4          WP5           LL_PWR_SetWakeUpPinPolarityLow
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  *         @arg @ref LL_PWR_WAKEUP_PIN5
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetWakeUpPinPolarityLow(uint32_t WakeUpPin)
{
  SET_BIT(PWR->CR4, WakeUpPin);
}

/**
  * @brief  Set the Wake-Up pin polarity high for the event detection
  * @rmtoll CR4          WP1           LL_PWR_SetWakeUpPinPolarityHigh\n
  *         CR4          WP2           LL_PWR_SetWakeUpPinPolarityHigh\n
  *         CR4          WP3           LL_PWR_SetWakeUpPinPolarityHigh\n
  *         CR4          WP4           LL_PWR_SetWakeUpPinPolarityHigh\n
  *         CR4          WP5           LL_PWR_SetWakeUpPinPolarityHigh
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  *         @arg @ref LL_PWR_WAKEUP_PIN5
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetWakeUpPinPolarityHigh(uint32_t WakeUpPin)
{
  CLEAR_BIT(PWR->CR4, WakeUpPin);
}

/**
  * @brief  Get the Wake-Up pin polarity for the event detection
  * @rmtoll CR4          WP1           LL_PWR_IsWakeUpPinPolarityLow\n
  *         CR4          WP2           LL_PWR_IsWakeUpPinPolarityLow\n
  *         CR4          WP3           LL_PWR_IsWakeUpPinPolarityLow\n
  *         CR4          WP4           LL_PWR_IsWakeUpPinPolarityLow\n
  *         CR4          WP5           LL_PWR_IsWakeUpPinPolarityLow
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  *         @arg @ref LL_PWR_WAKEUP_PIN5
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsWakeUpPinPolarityLow(uint32_t WakeUpPin)
{
  return (READ_BIT(PWR->CR4, WakeUpPin) == (WakeUpPin));
}

/**
  * @brief  Enable GPIO pull-up state in Standby and Shutdown modes
  * @rmtoll PUCRA        PU0-15        LL_PWR_EnableGPIOPullUp\n
  *         PUCRB        PU0-15        LL_PWR_EnableGPIOPullUp\n
  *         PUCRC        PU0-15        LL_PWR_EnableGPIOPullUp\n
  *         PUCRD        PU0-15        LL_PWR_EnableGPIOPullUp\n
  *         PUCRE        PU0-15        LL_PWR_EnableGPIOPullUp\n
  *         PUCRF        PU0-15        LL_PWR_EnableGPIOPullUp\n
  *         PUCRG        PU0-15        LL_PWR_EnableGPIOPullUp\n
  *         PUCRH        PU0-1         LL_PWR_EnableGPIOPullUp
  * @param  GPIO This parameter can be one of the following values:
  *         @arg @ref LL_PWR_GPIO_A
  *         @arg @ref LL_PWR_GPIO_B
  *         @arg @ref LL_PWR_GPIO_C
  *         @arg @ref LL_PWR_GPIO_D
  *         @arg @ref LL_PWR_GPIO_E
  *         @arg @ref LL_PWR_GPIO_F (*)
  *         @arg @ref LL_PWR_GPIO_G (*)
  *         @arg @ref LL_PWR_GPIO_H
  *
  *         (*) value not defined in all devices
  * @param  GPIONumber This parameter can be one of the following values:
  *         @arg @ref LL_PWR_GPIO_BIT_0
  *         @arg @ref LL_PWR_GPIO_BIT_1
  *         @arg @ref LL_PWR_GPIO_BIT_2
  *         @arg @ref LL_PWR_GPIO_BIT_3
  *         @arg @ref LL_PWR_GPIO_BIT_4
  *         @arg @ref LL_PWR_GPIO_BIT_5
  *         @arg @ref LL_PWR_GPIO_BIT_6
  *         @arg @ref LL_PWR_GPIO_BIT_7
  *         @arg @ref LL_PWR_GPIO_BIT_8
  *         @arg @ref LL_PWR_GPIO_BIT_9
  *         @arg @ref LL_PWR_GPIO_BIT_10
  *         @arg @ref LL_PWR_GPIO_BIT_11
  *         @arg @ref LL_PWR_GPIO_BIT_12
  *         @arg @ref LL_PWR_GPIO_BIT_13
  *         @arg @ref LL_PWR_GPIO_BIT_14
  *         @arg @ref LL_PWR_GPIO_BIT_15
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableGPIOPullUp(uint32_t GPIO, uint32_t GPIONumber)
{
  SET_BIT(*((uint32_t *)GPIO), GPIONumber);
}

/**
  * @brief  Disable GPIO pull-up state in Standby and Shutdown modes
  * @rmtoll PUCRA        PU0-15        LL_PWR_DisableGPIOPullUp\n
  *         PUCRB        PU0-15        LL_PWR_DisableGPIOPullUp\n
  *         PUCRC        PU0-15        LL_PWR_DisableGPIOPullUp\n
  *         PUCRD        PU0-15        LL_PWR_DisableGPIOPullUp\n
  *         PUCRE        PU0-15        LL_PWR_DisableGPIOPullUp\n
  *         PUCRF        PU0-15        LL_PWR_DisableGPIOPullUp\n
  *         PUCRG        PU0-15        LL_PWR_DisableGPIOPullUp\n
  *         PUCRH        PU0-1         LL_PWR_DisableGPIOPullUp
  * @param  GPIO This parameter can be one of the following values:
  *         @arg @ref LL_PWR_GPIO_A
  *         @arg @ref LL_PWR_GPIO_B
  *         @arg @ref LL_PWR_GPIO_C
  *         @arg @ref LL_PWR_GPIO_D
  *         @arg @ref LL_PWR_GPIO_E
  *         @arg @ref LL_PWR_GPIO_F (*)
  *         @arg @ref LL_PWR_GPIO_G (*)
  *         @arg @ref LL_PWR_GPIO_H
  *
  *         (*) value not defined in all devices
  * @param  GPIONumber This parameter can be one of the following values:
  *         @arg @ref LL_PWR_GPIO_BIT_0
  *         @arg @ref LL_PWR_GPIO_BIT_1
  *         @arg @ref LL_PWR_GPIO_BIT_2
  *         @arg @ref LL_PWR_GPIO_BIT_3
  *         @arg @ref LL_PWR_GPIO_BIT_4
  *         @arg @ref LL_PWR_GPIO_BIT_5
  *         @arg @ref LL_PWR_GPIO_BIT_6
  *         @arg @ref LL_PWR_GPIO_BIT_7
  *         @arg @ref LL_PWR_GPIO_BIT_8
  *         @arg @ref LL_PWR_GPIO_BIT_9
  *         @arg @ref LL_PWR_GPIO_BIT_10
  *         @arg @ref LL_PWR_GPIO_BIT_11
  *         @arg @ref LL_PWR_GPIO_BIT_12
  *         @arg @ref LL_PWR_GPIO_BIT_13
  *         @arg @ref LL_PWR_GPIO_BIT_14
  *         @arg @ref LL_PWR_GPIO_BIT_15
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableGPIOPullUp(uint32_t GPIO, uint32_t GPIONumber)
{
  CLEAR_BIT(*((uint32_t *)GPIO), GPIONumber);
}

/**
  * @brief  Check if GPIO pull-up state is enabled
  * @rmtoll PUCRA        PU0-15        LL_PWR_IsEnabledGPIOPullUp\n
  *         PUCRB        PU0-15        LL_PWR_IsEnabledGPIOPullUp\n
  *         PUCRC        PU0-15        LL_PWR_IsEnabledGPIOPullUp\n
  *         PUCRD        PU0-15        LL_PWR_IsEnabledGPIOPullUp\n
  *         PUCRE        PU0-15        LL_PWR_IsEnabledGPIOPullUp\n
  *         PUCRF        PU0-15        LL_PWR_IsEnabledGPIOPullUp\n
  *         PUCRG        PU0-15        LL_PWR_IsEnabledGPIOPullUp\n
  *         PUCRH        PU0-1         LL_PWR_IsEnabledGPIOPullUp
  * @param  GPIO This parameter can be one of the following values:
  *         @arg @ref LL_PWR_GPIO_A
  *         @arg @ref LL_PWR_GPIO_B
  *         @arg @ref LL_PWR_GPIO_C
  *         @arg @ref LL_PWR_GPIO_D
  *         @arg @ref LL_PWR_GPIO_E
  *         @arg @ref LL_PWR_GPIO_F (*)
  *         @arg @ref LL_PWR_GPIO_G (*)
  *         @arg @ref LL_PWR_GPIO_H
  *
  *         (*) value not defined in all devices
  * @param  GPIONumber This parameter can be one of the following values:
  *         @arg @ref LL_PWR_GPIO_BIT_0
  *         @arg @ref LL_PWR_GPIO_BIT_1
  *         @arg @ref LL_PWR_GPIO_BIT_2
  *         @arg @ref LL_PWR_GPIO_BIT_3
  *         @arg @ref LL_PWR_GPIO_BIT_4
  *         @arg @ref LL_PWR_GPIO_BIT_5
  *         @arg @ref LL_PWR_GPIO_BIT_6
  *         @arg @ref LL_PWR_GPIO_BIT_7
  *         @arg @ref LL_PWR_GPIO_BIT_8
  *         @arg @ref LL_PWR_GPIO_BIT_9
  *         @arg @ref LL_PWR_GPIO_BIT_10
  *         @arg @ref LL_PWR_GPIO_BIT_11
  *         @arg @ref LL_PWR_GPIO_BIT_12
  *         @arg @ref LL_PWR_GPIO_BIT_13
  *         @arg @ref LL_PWR_GPIO_BIT_14
  *         @arg @ref LL_PWR_GPIO_BIT_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledGPIOPullUp(uint32_t GPIO, uint32_t GPIONumber)
{
  return (READ_BIT(*((uint32_t *)(GPIO)), GPIONumber) == (GPIONumber));
}

/**
  * @brief  Enable GPIO pull-down state in Standby and Shutdown modes
  * @rmtoll PDCRA        PD0-15        LL_PWR_EnableGPIOPullDown\n
  *         PDCRB        PD0-15        LL_PWR_EnableGPIOPullDown\n
  *         PDCRC        PD0-15        LL_PWR_EnableGPIOPullDown\n
  *         PDCRD        PD0-15        LL_PWR_EnableGPIOPullDown\n
  *         PDCRE        PD0-15        LL_PWR_EnableGPIOPullDown\n
  *         PDCRF        PD0-15        LL_PWR_EnableGPIOPullDown\n
  *         PDCRG        PD0-15        LL_PWR_EnableGPIOPullDown\n
  *         PDCRH        PD0-1         LL_PWR_EnableGPIOPullDown
  * @param  GPIO This parameter can be one of the following values:
  *         @arg @ref LL_PWR_GPIO_A
  *         @arg @ref LL_PWR_GPIO_B
  *         @arg @ref LL_PWR_GPIO_C
  *         @arg @ref LL_PWR_GPIO_D
  *         @arg @ref LL_PWR_GPIO_E
  *         @arg @ref LL_PWR_GPIO_F (*)
  *         @arg @ref LL_PWR_GPIO_G (*)
  *         @arg @ref LL_PWR_GPIO_H
  *
  *         (*) value not defined in all devices
  * @param  GPIONumber This parameter can be one of the following values:
  *         @arg @ref LL_PWR_GPIO_BIT_0
  *         @arg @ref LL_PWR_GPIO_BIT_1
  *         @arg @ref LL_PWR_GPIO_BIT_2
  *         @arg @ref LL_PWR_GPIO_BIT_3
  *         @arg @ref LL_PWR_GPIO_BIT_4
  *         @arg @ref LL_PWR_GPIO_BIT_5
  *         @arg @ref LL_PWR_GPIO_BIT_6
  *         @arg @ref LL_PWR_GPIO_BIT_7
  *         @arg @ref LL_PWR_GPIO_BIT_8
  *         @arg @ref LL_PWR_GPIO_BIT_9
  *         @arg @ref LL_PWR_GPIO_BIT_10
  *         @arg @ref LL_PWR_GPIO_BIT_11
  *         @arg @ref LL_PWR_GPIO_BIT_12
  *         @arg @ref LL_PWR_GPIO_BIT_13
  *         @arg @ref LL_PWR_GPIO_BIT_14
  *         @arg @ref LL_PWR_GPIO_BIT_15
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableGPIOPullDown(uint32_t GPIO, uint32_t GPIONumber)
{
  register uint32_t temp = (uint32_t)(GPIO) + 4;
  SET_BIT(*((uint32_t *)(temp)), GPIONumber);
}

/**
  * @brief  Disable GPIO pull-down state in Standby and Shutdown modes
  * @rmtoll PDCRA        PD0-15        LL_PWR_DisableGPIOPullDown\n
  *         PDCRB        PD0-15        LL_PWR_DisableGPIOPullDown\n
  *         PDCRC        PD0-15        LL_PWR_DisableGPIOPullDown\n
  *         PDCRD        PD0-15        LL_PWR_DisableGPIOPullDown\n
  *         PDCRE        PD0-15        LL_PWR_DisableGPIOPullDown\n
  *         PDCRF        PD0-15        LL_PWR_DisableGPIOPullDown\n
  *         PDCRG        PD0-15        LL_PWR_DisableGPIOPullDown\n
  *         PDCRH        PD0-1         LL_PWR_DisableGPIOPullDown
  * @param  GPIO This parameter can be one of the following values:
  *         @arg @ref LL_PWR_GPIO_A
  *         @arg @ref LL_PWR_GPIO_B
  *         @arg @ref LL_PWR_GPIO_C
  *         @arg @ref LL_PWR_GPIO_D
  *         @arg @ref LL_PWR_GPIO_E
  *         @arg @ref LL_PWR_GPIO_F (*)
  *         @arg @ref LL_PWR_GPIO_G (*)
  *         @arg @ref LL_PWR_GPIO_H
  *
  *         (*) value not defined in all devices
  * @param  GPIONumber This parameter can be one of the following values:
  *         @arg @ref LL_PWR_GPIO_BIT_0
  *         @arg @ref LL_PWR_GPIO_BIT_1
  *         @arg @ref LL_PWR_GPIO_BIT_2
  *         @arg @ref LL_PWR_GPIO_BIT_3
  *         @arg @ref LL_PWR_GPIO_BIT_4
  *         @arg @ref LL_PWR_GPIO_BIT_5
  *         @arg @ref LL_PWR_GPIO_BIT_6
  *         @arg @ref LL_PWR_GPIO_BIT_7
  *         @arg @ref LL_PWR_GPIO_BIT_8
  *         @arg @ref LL_PWR_GPIO_BIT_9
  *         @arg @ref LL_PWR_GPIO_BIT_10
  *         @arg @ref LL_PWR_GPIO_BIT_11
  *         @arg @ref LL_PWR_GPIO_BIT_12
  *         @arg @ref LL_PWR_GPIO_BIT_13
  *         @arg @ref LL_PWR_GPIO_BIT_14
  *         @arg @ref LL_PWR_GPIO_BIT_15
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableGPIOPullDown(uint32_t GPIO, uint32_t GPIONumber)
{
  register uint32_t temp = (uint32_t)(GPIO) + 4;
  CLEAR_BIT(*((uint32_t *)(temp)), GPIONumber);
}

/**
  * @brief  Check if GPIO pull-down state is enabled
  * @rmtoll PDCRA        PD0-15        LL_PWR_IsEnabledGPIOPullDown\n
  *         PDCRB        PD0-15        LL_PWR_IsEnabledGPIOPullDown\n
  *         PDCRC        PD0-15        LL_PWR_IsEnabledGPIOPullDown\n
  *         PDCRD        PD0-15        LL_PWR_IsEnabledGPIOPullDown\n
  *         PDCRE        PD0-15        LL_PWR_IsEnabledGPIOPullDown\n
  *         PDCRF        PD0-15        LL_PWR_IsEnabledGPIOPullDown\n
  *         PDCRG        PD0-15        LL_PWR_IsEnabledGPIOPullDown\n
  *         PDCRH        PD0-1         LL_PWR_IsEnabledGPIOPullDown
  * @param  GPIO This parameter can be one of the following values:
  *         @arg @ref LL_PWR_GPIO_A
  *         @arg @ref LL_PWR_GPIO_B
  *         @arg @ref LL_PWR_GPIO_C
  *         @arg @ref LL_PWR_GPIO_D
  *         @arg @ref LL_PWR_GPIO_E
  *         @arg @ref LL_PWR_GPIO_F (*)
  *         @arg @ref LL_PWR_GPIO_G (*)
  *         @arg @ref LL_PWR_GPIO_H
  *
  *         (*) value not defined in all devices
  * @param  GPIONumber This parameter can be one of the following values:
  *         @arg @ref LL_PWR_GPIO_BIT_0
  *         @arg @ref LL_PWR_GPIO_BIT_1
  *         @arg @ref LL_PWR_GPIO_BIT_2
  *         @arg @ref LL_PWR_GPIO_BIT_3
  *         @arg @ref LL_PWR_GPIO_BIT_4
  *         @arg @ref LL_PWR_GPIO_BIT_5
  *         @arg @ref LL_PWR_GPIO_BIT_6
  *         @arg @ref LL_PWR_GPIO_BIT_7
  *         @arg @ref LL_PWR_GPIO_BIT_8
  *         @arg @ref LL_PWR_GPIO_BIT_9
  *         @arg @ref LL_PWR_GPIO_BIT_10
  *         @arg @ref LL_PWR_GPIO_BIT_11
  *         @arg @ref LL_PWR_GPIO_BIT_12
  *         @arg @ref LL_PWR_GPIO_BIT_13
  *         @arg @ref LL_PWR_GPIO_BIT_14
  *         @arg @ref LL_PWR_GPIO_BIT_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledGPIOPullDown(uint32_t GPIO, uint32_t GPIONumber)
{
  register uint32_t temp = (uint32_t)(GPIO) + 4;
  return (READ_BIT(*((uint32_t *)(temp)), GPIONumber) == (GPIONumber));
}

/**
  * @}
  */

/** @defgroup PWR_LL_EF_FLAG_Management FLAG_Management
  * @{
  */

/**
  * @brief  Get Internal Wake-up line Flag
  * @rmtoll SR1          WUFI          LL_PWR_IsActiveFlag_InternWU
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_InternWU(void)
{
  return (READ_BIT(PWR->SR1, PWR_SR1_WUFI) == (PWR_SR1_WUFI));
}

/**
  * @brief  Get Stand-By Flag
  * @rmtoll SR1          SBF           LL_PWR_IsActiveFlag_SB
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_SB(void)
{
  return (READ_BIT(PWR->SR1, PWR_SR1_SBF) == (PWR_SR1_SBF));
}

/**
  * @brief  Get Wake-up Flag 5
  * @rmtoll SR1          WUF5          LL_PWR_IsActiveFlag_WU5
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_WU5(void)
{
  return (READ_BIT(PWR->SR1, PWR_SR1_WUF5) == (PWR_SR1_WUF5));
}

/**
  * @brief  Get Wake-up Flag 4
  * @rmtoll SR1          WUF4          LL_PWR_IsActiveFlag_WU4
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_WU4(void)
{
  return (READ_BIT(PWR->SR1, PWR_SR1_WUF4) == (PWR_SR1_WUF4));
}

/**
  * @brief  Get Wake-up Flag 3
  * @rmtoll SR1          WUF3          LL_PWR_IsActiveFlag_WU3
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_WU3(void)
{
  return (READ_BIT(PWR->SR1, PWR_SR1_WUF3) == (PWR_SR1_WUF3));
}

/**
  * @brief  Get Wake-up Flag 2
  * @rmtoll SR1          WUF2          LL_PWR_IsActiveFlag_WU2
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_WU2(void)
{
  return (READ_BIT(PWR->SR1, PWR_SR1_WUF2) == (PWR_SR1_WUF2));
}

/**
  * @brief  Get Wake-up Flag 1
  * @rmtoll SR1          WUF1          LL_PWR_IsActiveFlag_WU1
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_WU1(void)
{
  return (READ_BIT(PWR->SR1, PWR_SR1_WUF1) == (PWR_SR1_WUF1));
}

/**
  * @brief  Clear Stand-By Flag
  * @rmtoll SCR          CSBF          LL_PWR_ClearFlag_SB
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_SB(void)
{
  WRITE_REG(PWR->SCR, PWR_SCR_CSBF);
}

/**
  * @brief  Clear Wake-up Flags
  * @rmtoll SCR          CWUF          LL_PWR_ClearFlag_WU
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_WU(void)
{
  WRITE_REG(PWR->SCR, PWR_SCR_CWUF);
}

/**
  * @brief  Clear Wake-up Flag 5
  * @rmtoll SCR          CWUF5         LL_PWR_ClearFlag_WU5
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_WU5(void)
{
  WRITE_REG(PWR->SCR, PWR_SCR_CWUF5);
}

/**
  * @brief  Clear Wake-up Flag 4
  * @rmtoll SCR          CWUF4         LL_PWR_ClearFlag_WU4
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_WU4(void)
{
  WRITE_REG(PWR->SCR, PWR_SCR_CWUF4);
}

/**
  * @brief  Clear Wake-up Flag 3
  * @rmtoll SCR          CWUF3         LL_PWR_ClearFlag_WU3
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_WU3(void)
{
  WRITE_REG(PWR->SCR, PWR_SCR_CWUF3);
}

/**
  * @brief  Clear Wake-up Flag 2
  * @rmtoll SCR          CWUF2         LL_PWR_ClearFlag_WU2
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_WU2(void)
{
  WRITE_REG(PWR->SCR, PWR_SCR_CWUF2);
}

/**
  * @brief  Clear Wake-up Flag 1
  * @rmtoll SCR          CWUF1         LL_PWR_ClearFlag_WU1
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_WU1(void)
{
  WRITE_REG(PWR->SCR, PWR_SCR_CWUF1);
}

/**
  * @brief  Indicate whether VDDA voltage is below or above PVM4 threshold
  * @rmtoll SR2          PVMO4         LL_PWR_IsActiveFlag_PVMO4
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_PVMO4(void)
{
  return (READ_BIT(PWR->SR2, PWR_SR2_PVMO4) == (PWR_SR2_PVMO4));
}

/**
  * @brief  Indicate whether VDDA voltage is below or above PVM3 threshold
  * @rmtoll SR2          PVMO3         LL_PWR_IsActiveFlag_PVMO3
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_PVMO3(void)
{
  return (READ_BIT(PWR->SR2, PWR_SR2_PVMO3) == (PWR_SR2_PVMO3));
}

#if defined(PWR_SR2_PVMO2)
/**
  * @brief  Indicate whether VDDIO2 voltage is below or above PVM2 threshold
  * @rmtoll SR2          PVMO2         LL_PWR_IsActiveFlag_PVMO2
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_PVMO2(void)
{
  return (READ_BIT(PWR->SR2, PWR_SR2_PVMO2) == (PWR_SR2_PVMO2));
}
#endif /* PWR_SR2_PVMO2 */

#if defined(USB_OTG_FS)
/**
  * @brief  Indicate whether VDDUSB voltage is below or above PVM1 threshold
  * @rmtoll SR2          PVMO1         LL_PWR_IsActiveFlag_PVMO1
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_PVMO1(void)
{
  return (READ_BIT(PWR->SR2, PWR_SR2_PVMO1) == (PWR_SR2_PVMO1));
}
#endif /* USB_OTG_FS */

/**
  * @brief  Indicate whether VDD voltage is below or above the selected PVD threshold
  * @rmtoll SR2          PVDO          LL_PWR_IsActiveFlag_PVDO
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_PVDO(void)
{
  return (READ_BIT(PWR->SR2, PWR_SR2_PVDO) == (PWR_SR2_PVDO));
}

/**
  * @brief  Indicate whether the regulator is ready in the selected voltage range or if its output voltage is still changing to the required voltage level
  * @rmtoll SR2          VOSF          LL_PWR_IsActiveFlag_VOSF
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_VOSF(void)
{
  return (READ_BIT(PWR->SR2, PWR_SR2_VOSF) == (PWR_SR2_VOSF));
}

/**
  * @brief  Indicate whether the regulator is ready in main mode or is in low-power mode
  * @note: Take care, return value "0" means the regulator is ready.  Return value "1" means the output voltage range is still changing.
  * @rmtoll SR2          REGLPF        LL_PWR_IsActiveFlag_REGLPF
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_REGLPF(void)
{
  return (READ_BIT(PWR->SR2, PWR_SR2_REGLPF) == (PWR_SR2_REGLPF));
}

/**
  * @brief  Indicate whether or not the low-power regulator is ready
  * @rmtoll SR2          REGLPS        LL_PWR_IsActiveFlag_REGLPS
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_REGLPS(void)
{
  return (READ_BIT(PWR->SR2, PWR_SR2_REGLPS) == (PWR_SR2_REGLPS));
}

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup PWR_LL_EF_Init De-initialization function
  * @{
  */
ErrorStatus LL_PWR_DeInit(void);
/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

#endif /* defined(PWR) */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_LL_PWR_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
