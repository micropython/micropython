/**
  ******************************************************************************
  * @file    stm32n6xx_hal_pwr_ex.c
  * @author  MCD Application Team
  * @brief   Extended PWR HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Power Controller extension peripheral :
  *           + Power Supply Control Functions
  *           + Low Power Control Functions
  *           + Voltage Monitoring Functions
  *           + Memories Retention Functions
  *           + I/O Pull-Up Pull-Down Configuration Functions
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
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
   (#) Call HAL_PWREx_ConfigSupply() function to configure the regulator supply
       with the following different setups according to hardware:
       (+) PWR_SMPS_SUPPLY
       (+) PWR_EXTERNAL_SOURCE_SUPPLY

   (#) Call HAL_PWREx_GetSupplyConfig() function to get the current supply setup.

   (#) Call HAL_PWREx_ControlVoltageScaling() function to configure the main
       internal regulator output voltage. The voltage scaling could be one of
       the following scales :
       (+) PWR_REGULATOR_VOLTAGE_SCALE0
       (+) PWR_REGULATOR_VOLTAGE_SCALE1

   (#) Call HAL_PWREx_GetVoltageRange() function to get the current output
       voltage applied to the main regulator.

   (#) Call HAL_PWREx_ControlStopModeVoltageScaling() function to configure the main
       internal regulator output voltage in STOP mode. The voltage scaling could be one of
       the following scales :
       (+) PWR_REGULATOR_STOP_VOLTAGE_SCALE3
       (+) PWR_REGULATOR_STOP_VOLTAGE_SCALE5

   (#) Call HAL_PWREx_GetStopModeVoltageRange() function to get the current output
       voltage applied to the main regulator in STOP mode.

   (#) Call HAL_PWREx_EnableWakeUpPin() functions to enable the Wake-up pin
       functionality for the selected pin.

   (#) Call HAL_PWREx_EnableBkupRAMRetention() and HAL_PWREx_DisableBkupRAMRetention()
       functions to enable and disable BKPSRAM retention in Standby mode.

   (#) Call HAL_PWREx_EnableTCMRetention() and HAL_PWREx_DisableTCMRetention() functions
       to enable and disable the I-TCM and D-TCM RAMs retention in Standby mode.

   (#) Call HAL_PWREx_EnableTCMFLXRetention() and HAL_PWREx_DisableTCMFLXRetention()
       functions to enable and disable the I-TCM FLEXMEM retention in Standby mode.

   (#) Call HAL_PWREx_SetPulseLow() function to configure the minimum guaranteed
       duration of the pwr_on low pulse in Standby mode ( between ~ 0 -> 31 ms).

   (#) Call HAL_PWREx_GetPulseLow() function to get the minimum guaranteed
       duration of the pwr_on low pulse in Standby mode.

   (#) Call HAL_PWREx_EnableTCMFLXRetention() and HAL_PWREx_DisableTCMFLXRetention()
       functions to enable and disable the I-TCM FLEXMEM retention in Standby mode.

   (#) Call HAL_PWREx_EnableSMPSPWM() and HAL_PWREx_DisableSMPSPWM() functions to
       enable and disable the SMPS low-power mode in Stop SVOS range 3.

   (#) Call HAL_PWREx_EnablePullDownOutput() and HAL_PWREx_DisablePullDownOutput()
       functions to enable and disable the pull down on output voltage during
       power-down mode.

   (#) Call HAL_PWREx_EnableMonitoring() and HAL_PWREx_DisableMonitoring()
       functions to enable and disable the VBAT and Temperature monitoring.
       When VBAT and Temperature monitoring feature is enables, use
       HAL_PWREx_GetTemperatureLevel() and HAL_PWREx_GetVBATLevel() to get
       respectively the Temperature level and VBAT level.

   (#) Call HAL_PWREx_EnableVDDCOREMonitoring() and HAL_PWREx_EnableVDDCOREMonitoring()
       functions to enable and disable the VCORE monitoring.
       When VDDCORE monitoring feature is enables, use HAL_PWREx_GetVDDCORELevel()
       function to get the current VDDCORE level.

   (#) Call HAL_PWREx_ConfigVDDCORELevel() function to configure the VDDCORE
       voltage detector low-level selection.

   (#) Call HAL_PWREx_ConfigVddIORange() functions to configure the Vdd IO range:
       (+) PWR_VDDIO
       (+) PWR_VDDIO2
       (+) PWR_VDDIO3
       (+) PWR_VDDIO4
       (+) PWR_VDDIO5
       The voltage range setting could be one of the following values:
       (+) PWR_VDDIO_RANGE_3V3
       (+) PWR_VDDIO_RANGE_1V8

   (#) Call HAL_PWREx_GetVddIORange() function to get the current
       voltage range setting on the selected Vdd IO.

   (#) Call HAL_PWREx_EnableVddIO4RangeSTBY() and HAL_PWREx_DisableVddIO4RangeSTBY()
       functions to enable and disable the VDD IO4 voltage range Standby mode.

   (#) Call HAL_PWREx_EnableVddIO5RangeSTBY() and HAL_PWREx_DisableVddIO5RangeSTBY()
       functions to enable and disable the VDD IO5 voltage range Standby mode.

   (#) Call HAL_PWREx_EnableVddUSB() and HAL_PWREx_DisableVddUSB()
       functions to enable and disable the VDD33USB supply valid.

   (#) Call HAL_PWREx_EnableVddIO2() and HAL_PWREx_DisableVddIO2()
       functions to enable and disable the VDDIO2 supply valid.

   (#) Call HAL_PWREx_EnableVddIO3() and HAL_PWREx_DisableVddIO3()
       functions to enable and disable the VDDIO3 supply valid.

   (#) Call HAL_PWREx_EnableVddIO4() and HAL_PWREx_DisableVddIO4()
       functions to enable and disable the VDDIO4 supply valid.

   (#) Call HAL_PWREx_EnableVddIO5() and HAL_PWREx_DisableVddIO5()
       functions to enable and disable the VDDIO5 supply valid.

   (#) Call HAL_PWREx_EnableVddA() and HAL_PWREx_DisableVddA()
       functions to enable and disable the VDDA18ADC supply valid.

   (#) Call HAL_PWREx_ConfigPVM() after setting parameters to be configured
       (event mode and PVD type) in order to set up the Peripheral Voltage
       Monitor, then use HAL_PWREx_EnableVddUSBVMEN(), HAL_PWREx_EnableVddIO2VMEN(),
       HAL_PWREx_EnableVddIO3VMEN(), HAL_PWREx_EnableVddIO4VMEN(),
       HAL_PWREx_EnableVddIO5VMEN() and HAL_PWREx_EnableVddAVMEN() functions to
       start the PVM VDDx monitoring.
       Use HAL_PWREx_DisableVddUSBVMEN(), HAL_PWREx_DisableVddIO2VMEN(),
       HAL_PWREx_DisableVddIO3VMEN(), HAL_PWREx_DisableVddIO4VMEN(),
       HAL_PWREx_DisableVddIO5VMEN() and HAL_PWREx_DisableVddAVMEN() to stop
       the PVM VDDx monitoring.
       (+) PVM monitored voltages are :
             (++) VDDUSB
             (++) VDDIO2
             (++) VDDIO3
             (++) VDDIO4
             (++) VDDIO5
             (++) VDDA

   (#) Call HAL_PWREx_PVD_PVM_IRQHandler() function to handle the PWR PVD and
       PVM interrupt request.

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup PWREx PWREx
  * @brief    PWR Extended HAL module driver
  * @{
  */

#if defined (HAL_PWR_MODULE_ENABLED)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** @defgroup PWREx_REG_SET_TIMEOUT PWR Extended Flag Setting Time Out Value
  * @{
  */
#define PWR_FLAG_SETTING_DELAY         (1000U)
/**
  * @}
  */

/** @defgroup PWREx_WakeUp_Pins_Offsets PWREx Wake-Up Pins masks and offsets
  * @{
  */
/* Wake-Up Pins EXTI register mask */
#define PWR_EXTI_WAKEUP_PINS_MASK      (EXTI_IMR2_IM60 | EXTI_IMR2_IM61 |\
                                        EXTI_IMR2_IM62 | EXTI_IMR2_IM63 )

/* Wake-Up Pins PWR Pin Pull shift offsets */
#define PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET (2U)
/**
  * @}
  */

/** @defgroup PWR_PVM_Mode_Mask PWR PVM Mode Mask
  * @{
  */
#define PVM_RISING_EDGE                (1U)                                    /*!< Mask for rising edge set as PVM trigger                 */
#define PVM_FALLING_EDGE               (2U)                                    /*!< Mask for falling edge set as PVM trigger                */
#define PVM_MODE_IT                    (4U)                                    /*!< Mask for interruption yielded by PVM threshold crossing */
#define PVM_MODE_EVT                   (8U)                                    /*!< Mask for event yielded by PVM threshold crossing        */
/**
  * @}
  */

/** @defgroup PWR_VDDCOREVM_Mode_Mask PWR VddCORE VM Mode Mask
  * @{
  */
#define VDDCOREVM_RISING_EDGE          (1U)                                    /*!< Mask for rising edge set as PVM trigger                 */
#define VDDCOREVM_FALLING_EDGE         (2U)                                    /*!< Mask for falling edge set as PVM trigger                */
#define VDDCOREVM_MODE_IT              (4U)                                    /*!< Mask for interruption yielded by PVM threshold crossing */
#define VDDCOREVM_MODE_EVT             (8U)                                    /*!< Mask for event yielded by PVM threshold crossing        */
/**
  * @}
  */



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup PWREx_Exported_Functions PWR Extended Exported Functions
  * @{
  */

/** @defgroup PWREx_Exported_Functions_Group1 Power Supply Control Functions
  * @brief    Power supply control functions
  *
@verbatim
 ===============================================================================
                  ##### Power supply control functions #####
 ===============================================================================
    [..]
       This section provides functions allowing to control power supply.

   (+) The STM32N6 series devices allows two different supply configurations:
       Direct SMPS supply and external supply.

   (#) After a system reset, the software must configure the used supply configuration
       in PWR_CR1 using  HAL_PWREx_ConfigSupply() function before changing VOS in PWR_VOSCR,
       or the RCC sys_ck frequency.

@endverbatim
  * @{
  */

/**
  * @brief Configure the system Power Supply.
  * @param  SupplySource : Specifies the Power Supply source to set after a
  *                        system startup.
  *         This parameter can be one of the following values :
  *            @arg @ref PWR_SMPS_SUPPLY            : VCORE power domains are supplied
  *                                                   from SMPS step-down converter
  *                                                   according to VOS.
  *            @arg @ref PWR_EXTERNAL_SOURCE_SUPPLY : VCORE supplied from external source.
  *                                                   SMPS step-down converter disabled.
  * @note: The power supply configuration can be written only one time after POR.
  * @note: The power supply configuration is not reset by wakeup from Standby mode and
  *        application reset, but only reset by VDD POR.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_PWREx_ConfigSupply(uint32_t SupplySource)
{
  uint32_t tickstart;

  /* Check the parameters */
  assert_param(IS_PWR_SUPPLY(SupplySource));

  /* Set the power supply configuration */
  MODIFY_REG(PWR->CR1, PWR_SUPPLY_CONFIG_MASK, SupplySource);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait till voltage level flag is set */
  while ((PWR->VOSCR & PWR_VOSCR_ACTVOSRDY) == 0U)
  {
    if ((HAL_GetTick() - tickstart) > PWR_FLAG_SETTING_DELAY)
    {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @brief Get the power supply configuration.
  * @retval The supply configuration.
  */
uint32_t HAL_PWREx_GetSupplyConfig(void)
{
  return (PWR->CR1 & PWR_SUPPLY_CONFIG_MASK);
}

/**
  * @brief Configure the main internal regulator output voltage.
  * @param  VoltageScaling : Specifies the regulator output voltage to achieve
  *                          a tradeoff between performance and power
  *                          consumption.
  *          This parameter can be one of the following values :
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE0 : Regulator voltage scaling range 0 (highest frequency).
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE1 : Regulator voltage scaling range 1 (lowest power).
  * @note After reset, the system starts from HSI with VOS low.
  * @note When increasing the performance, the voltage scaling must be changed
  *       before increasing the system frequency.
  *       When decreasing performance, the system frequency must first be decreased
  *       before changing the voltage scaling.
  * @note When exiting from Stop mode or Standby mode, the Run mode voltage
  *       scaling is reset to the default VOS low value.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling(uint32_t VoltageScaling)
{
  uint32_t tickstart;

  /* Check the parameters */
  assert_param(IS_PWR_REGULATOR_VOLTAGE(VoltageScaling));

  /* Set the voltage range */
  MODIFY_REG(PWR->VOSCR, PWR_VOSCR_VOS, VoltageScaling);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait till voltage level flag is set */
  while ((PWR->VOSCR & PWR_VOSCR_VOSRDY) == 0U)
  {
    if ((HAL_GetTick() - tickstart) > PWR_FLAG_SETTING_DELAY)
    {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @brief Get the main internal regulator output voltage. Reflecting the last
  *        VOS value applied to the PMU.
  * @retval The current applied VOS selection.
  */
uint32_t HAL_PWREx_GetVoltageRange(void)
{
  /* Get the active voltage scaling */
  return ((PWR->VOSCR & PWR_VOSCR_ACTVOS) >> PWR_VOSCR_ACTVOS_Pos);
}

/**
  * @brief Configure the main internal regulator output voltage in STOP mode.
  * @param  VoltageScaling : Specifies the regulator output voltage when the
  *         system enters Stop mode to achieve a tradeoff between performance
  *         and power consumption.
  *          This parameter can be one of the following values:
  *            @arg PWR_REGULATOR_STOP_VOLTAGE_SCALE3 : System Stop mode voltage scaling range 3 (highest frequency).
  *            @arg PWR_REGULATOR_STOP_VOLTAGE_SCALE5 : System Stop mode voltage scaling range 5 (lowest power).
  * @retval None.
  */
void HAL_PWREx_ControlStopModeVoltageScaling(uint32_t VoltageScaling)
{
  /* Check the parameters */
  assert_param(IS_PWR_STOP_MODE_REGULATOR_VOLTAGE(VoltageScaling));

  /* Return the stop mode voltage range */
  MODIFY_REG(PWR->CPUCR, PWR_CPUCR_SVOS, VoltageScaling);
}

/**
  * @brief Get the main internal regulator output voltage in STOP mode.
  * @retval The actual applied SVOS selection.
  */
uint32_t HAL_PWREx_GetStopModeVoltageRange(void)
{
  /* Return the stop voltage scaling */
  return (PWR->CPUCR & PWR_CPUCR_SVOS);
}
/**
  * @}
  */

/** @defgroup PWREx_Exported_Functions_Group2 Wakeup Pins configuration functions
  * @brief     Wakeup Pins configuration functions
  *
@verbatim
 ===============================================================================
                     ##### Wakeup Pins configuration functions #####
 ===============================================================================
    [..]

    *** Wakeup Pins configuration ****
    ===================================
    [..]
      Wakeup pins allow the system to exit from Standby mode. The configuration
      of wakeup pins is done with the HAL_PWREx_EnableWakeUpPin(sPinParams)
      function with:
       (+) sPinParams: structure to enable and configure a wakeup pin:
        (++) WakeUpPin: Wakeup pin to be enabled.
        (++) PinPolarity: Wakeup pin polarity (rising or falling edge).
        (++) PinPull: Wakeup pin pull (no pull, pull-up or pull-down).
    [..]
      The wakeup pins are internally connected to the EXTI lines [60-63] to
      generate an interrupt if enabled.
    [..]
      When a wakeup pin event is received the HAL_PWR_WAKEUP_PIN_IRQHandler is
      called and the appropriate flag is set in the PWR_WKUPSR register. Then in
      the HAL_PWR_WAKEUP_PIN_IRQHandler function the wakeup pin flag will be
      cleared and the appropriate user callback will be called. The user can add
      his own code by customization of function pointer HAL_PWR_WKUPx_Callback.

@endverbatim
  * @{
  */
/**
  * @brief Enable the Wake-up PINx functionality.
  * @param  sPinParams : Pointer to a PWREx_WakeupPinTypeDef structure that
  *                      contains the configuration information for the wake-up
  *                      Pin.
  * @retval None.
  */
void HAL_PWREx_EnableWakeUpPin(const PWREx_WakeupPinTypeDef *sPinParams)
{
  uint32_t pinConfig;
  uint32_t regMask;
  const uint32_t pullMask = PWR_WKUPEPR_WKUPPUPD1;

  /* Check the parameters */
  assert_param(IS_PWR_WAKEUP_PIN(sPinParams->WakeUpPin));
  assert_param(IS_PWR_WAKEUP_PIN_POLARITY(sPinParams->PinPolarity));
  assert_param(IS_PWR_WAKEUP_PIN_PULL(sPinParams->PinPull));

  pinConfig = sPinParams->WakeUpPin | \
              (sPinParams->PinPolarity << ((POSITION_VAL(sPinParams->WakeUpPin) + PWR_WKUPEPR_WKUPP1_Pos) & 0x1FU)) | \
              (sPinParams->PinPull << (((POSITION_VAL(sPinParams->WakeUpPin) * PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET) + \
                                        PWR_WKUPEPR_WKUPPUPD1_Pos) & 0x1FU));

  regMask   = sPinParams->WakeUpPin | \
              (PWR_WKUPEPR_WKUPP1 << (POSITION_VAL(sPinParams->WakeUpPin) & 0x1FU)) | \
              (pullMask << ((POSITION_VAL(sPinParams->WakeUpPin) * PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET) & 0x1FU));

  /* Enable and Specify the Wake-Up pin polarity and the pull configuration
     for the event detection (rising or falling edge) */
  MODIFY_REG(PWR->WKUPEPR, regMask, pinConfig);

  /* Configure the Wakeup Pin EXTI Line */
  MODIFY_REG(EXTI->IMR2, PWR_EXTI_WAKEUP_PINS_MASK, (sPinParams->WakeUpPin << EXTI_IMR2_IM60_Pos));
}
/**
  * @}
  */


/** @defgroup PWREx_Exported_Functions_Group3 Memories Retention Functions
  * @brief    Memories Retention Functions
  *
@verbatim
 ===============================================================================
                     ##### Memories Retention Functions #####
 ===============================================================================
    [..]

    *** Backup RAM (BKPSRAM) retention ****
    =======================================
    [..]
      By default the content of BKPSRAM memories aren't retained in Standby mode.
      By setting the BKPRBSEN bit in the PWR_BDCR2 register using the
      HAL_PWREx_EnableBkupRAMRetention() function, the content of these memories
      are retained even in Standby mode.

    *** TCM RAMs and I-TCM FLEXMEM retention ****
    =======================================
    [..]
      By default the content of TCM RAMs and I-TCM FLEXMEM aren't retained in
      Standby mode.

      By setting the TCMRBSEN bit in the PWR_CR4 register using the
      HAL_PWREx_EnableTCMRetention() function, the content of these memories
      are retained even in Standby mode.

      By setting the TCMFLXRBSEN bit in the PWR_CR4 register using the
      HAL_PWREx_EnableTCMFLXRetention () function, the content of these memories
      are retained even in Standby mode.

@endverbatim
  * @{
  */

/**
  * @brief  Enable the Backup RAM retention in Standby and VBAT modes.
  * @note  After reset, PWR_BDCR2 is write-protected.
  *        DBP must be set in PWR_DBPCR to disable the write protection.
  * @retval None.
  */
void HAL_PWREx_EnableBkupRAMRetention(void)
{
  SET_BIT(PWR->BDCR2, PWR_BDCR2_BKPRBSEN);
}

/**
  * @brief  Disable the Backup RAM retention in Standby and VBAT modes.
  * @note  After reset, PWR_BDCR2 is write-protected.
  *        DBP must be set in PWR_DBPCR to disable the write protection.
  * @retval None.
  */
void HAL_PWREx_DisableBkupRAMRetention(void)
{
  CLEAR_BIT(PWR->BDCR2, PWR_BDCR2_BKPRBSEN);
}

/**
  * @brief Enable the TCM RAMs retention in Standby mode.
  * @retval None.
  */
void HAL_PWREx_EnableTCMRetention(void)
{
  SET_BIT(PWR->CR4, PWR_CR4_TCMRBSEN);
}

/**
  * @brief Disable the TCM RAMs retention in Standby mode.
  * @retval None.
  */
void HAL_PWREx_DisableTCMRetention(void)
{
  CLEAR_BIT(PWR->CR4, PWR_CR4_TCMRBSEN);
}

/**
  * @brief Enable the I-TCM FLEXMEM retention in Standby mode.
  * @retval None.
  */
void HAL_PWREx_EnableTCMFLXRetention(void)
{
  SET_BIT(PWR->CR4, PWR_CR4_TCMFLXRBSEN);
}

/**
  * @brief Disable the I-TCM FLEXMEM retention in Standby mode.
  * @retval None.
  */
void HAL_PWREx_DisableTCMFLXRetention(void)
{
  CLEAR_BIT(PWR->CR4, PWR_CR4_TCMFLXRBSEN);
}
/**
  * @}
  */


/** @defgroup PWREx_Exported_Functions_Group4 Low Power Control Functions
  * @brief    Low Power Control Functions
  *
@verbatim
 ===============================================================================
                     ##### Low Power Control Functions #####
 ===============================================================================
    [..]
@endverbatim
  * @{
  */

/**
  * @brief Configure the pwr_on pulse low configuration.
  * @param  Pulselowtime : Specifies minimum guaranteed duration of
                           the pwr_on low pulse in Standby mode.
  *          This parameter can be a value between 0 and 31:
  *            0: No guaranteed minimum low time.
  *            1: 1* 32 LSI cycles= ~ 1 ms guaranteed minimum low time.
  *            X: X* 32 LSI cycles= ~ X ms guaranteed minimum low time.
  * @retval None.
  */
void HAL_PWREx_SetPulseLow(uint32_t Pulselowtime)
{
  assert_param(IS_PWR_PULSE_LOW_TIME(Pulselowtime));

  /* Set the pwr_on pulse low configuration */
  MODIFY_REG(PWR->CR1, PWR_CR1_POPL_Msk, (Pulselowtime << PWR_CR1_POPL_Pos));
}

/**
  * @brief Get the pwr_on pulse low configuration.
  * @retval The actual applied minimum guaranteed duration of the
  *         pwr_on low pulse in Standby mode.
  */
uint32_t HAL_PWREx_GetPulseLow(void)
{
  /* Return the stop voltage scaling */
  return ((PWR->CR1 & PWR_CR1_POPL_Msk) >> PWR_CR1_POPL_Pos);
}

/**
  * @brief  Enable SMPS low-power mode (SVOS range 3 only).
  * @note   LPDS08V bit is used to keep the SMPS in PWM mode (MR) in Stop SVOS range 3.
  * @retval None.
  */
void HAL_PWREx_EnableSMPSPWM(void)
{
  SET_BIT(PWR->CR1, PWR_CR1_LPDS08V);
}

/**
  * @brief  Disable SMPS low-power mode (SVOS range 3 only).
  * @retval None.
  */
void HAL_PWREx_DisableSMPSPWM(void)
{
  CLEAR_BIT(PWR->CR1, PWR_CR1_LPDS08V);
}

/**
  * @brief  Enable the pull down on output voltage during power-down mode.
  * @retval None.
  */
void HAL_PWREx_EnablePullDownOutput(void)
{
  SET_BIT(PWR->CR1, PWR_CR1_MODE_PDN);
}

/**
  * @brief  Disable the pull down on output voltage during power-down mode.
  * @retval None.
  */
void HAL_PWREx_DisablePullDownOutput(void)
{
  CLEAR_BIT(PWR->CR1, PWR_CR1_MODE_PDN);
}
/**
  * @}
  */

/** @defgroup PWREx_Exported_Functions_Group5 Power Monitoring functions
  * @brief    Power Monitoring functions
  *
@verbatim
 ===============================================================================
                 ##### Power Monitoring functions #####
 ===============================================================================

    *** VBAT and Temperature supervision ***
    ========================================
    [..]
      (+) The VBAT battery voltage supply can be monitored by comparing it with
          two threshold levels: VBAThigh and VBATlow. VBATH flag and VBATL flags
          in the PWR control register 2 (PWR_BDCR1), indicate if VBAT is higher or
          lower than the threshold.
      (+) The temperature can be monitored by comparing it with two threshold
          levels, TEMPhigh and TEMPlow. TEMPH and TEMPL flags, in the PWR
          control register 2 (PWR_BDCR1), indicate whether the device temperature
          is higher or lower than the threshold.
      (+) The VBAT and the temperature monitoring is enabled by
          HAL_PWREx_EnableMonitoring() function and disabled by
          HAL_PWREx_DisableMonitoring() function.
      (+) The HAL_PWREx_GetVBATLevel() function returns the VBAT level which can
          be : PWR_VBAT_BELOW_LOW_THRESHOLD or PWR_VBAT_ABOVE_HIGH_THRESHOLD or
          PWR_VBAT_BETWEEN_HIGH_LOW_THRESHOLD.
      (+) The HAL_PWREx_GetTemperatureLevel() function returns the Temperature
          level which can be :
          PWR_TEMP_BELOW_LOW_THRESHOLD or PWR_TEMP_ABOVE_HIGH_THRESHOLD or
          PWR_TEMP_BETWEEN_HIGH_LOW_THRESHOLD.

@endverbatim
  * @{
  */

/**
  * @brief Enable the VBAT and temperature monitoring.
  * @note  After reset, PWR_BDCR1 is write-protected.
  *        DBP must be set in PWR_DBPCR to disable the write protection.
  * @retval HAL status.
  */
void HAL_PWREx_EnableMonitoring(void)
{
  /* Enable the VBAT and Temperature monitoring */
  SET_BIT(PWR->BDCR1, PWR_BDCR1_MONEN);
}

/**
  * @brief Disable the VBAT and temperature monitoring.
  * @note  After reset, PWR_BDCR1 is write-protected.
  *        DBP must be set in PWR_DBPCR to disable the write protection.
  * @retval HAL status.
  */
void HAL_PWREx_DisableMonitoring(void)
{
  /* Disable the VBAT and Temperature monitoring */
  CLEAR_BIT(PWR->BDCR1, PWR_BDCR1_MONEN);
}

/**
  * @brief Indicate whether the junction temperature is between, above or below
  *        the thresholds.
  * @retval Temperature level.
  */
uint32_t HAL_PWREx_GetTemperatureLevel(void)
{
  uint32_t tempLevel;
  uint32_t regValue;

  /* Read the temperature flags */
  regValue = READ_BIT(PWR->BDCR1, (PWR_BDCR1_TEMPH | PWR_BDCR1_TEMPL));

  /* Check if the temperature is below the threshold */
  if (regValue == PWR_BDCR1_TEMPL)
  {
    tempLevel = PWR_TEMP_BELOW_LOW_THRESHOLD;
  }
  /* Check if the temperature is above the threshold */
  else if (regValue == PWR_BDCR1_TEMPH)
  {
    tempLevel = PWR_TEMP_ABOVE_HIGH_THRESHOLD;
  }
  /* The temperature is between the thresholds */
  else
  {
    tempLevel = PWR_TEMP_BETWEEN_HIGH_LOW_THRESHOLD;
  }

  return tempLevel;
}

/**
  * @brief Indicate whether the Battery voltage level is between, above or below
  *        the thresholds.
  * @retval VBAT level.
  */
uint32_t HAL_PWREx_GetVBATLevel(void)
{
  uint32_t vbatLevel;
  uint32_t regValue;

  /* Read the VBAT flags */
  regValue = READ_BIT(PWR->BDCR1, (PWR_BDCR1_VBATH | PWR_BDCR1_VBATL));

  /* Check if the VBAT is below the threshold */
  if (regValue == PWR_BDCR1_VBATL)
  {
    vbatLevel = PWR_VBAT_BELOW_LOW_THRESHOLD;
  }
  /* Check if the VBAT is above the threshold */
  else if (regValue == PWR_BDCR1_VBATH)
  {
    vbatLevel = PWR_VBAT_ABOVE_HIGH_THRESHOLD;
  }
  /* The VBAT is between the thresholds */
  else
  {
    vbatLevel = PWR_VBAT_BETWEEN_HIGH_LOW_THRESHOLD;
  }

  return vbatLevel;
}

/**
  * @brief Enable the VDDCORE monitoring.
  * @retval HAL status.
  */
void HAL_PWREx_EnableVDDCOREMonitoring(void)
{
  /* Enable the VDDCORE monitoring */
  SET_BIT(PWR->CR3, PWR_CR3_VCOREMONEN);
}

/**
  * @brief Disable the VDDCORE monitoring.
  * @retval HAL status.
  */
void HAL_PWREx_DisableVDDCOREMonitoring(void)
{
  /* Disable the VDDCORE monitoring */
  CLEAR_BIT(PWR->CR3, PWR_CR3_VCOREMONEN);
}

/**
  * @brief  Configure the VDDCORE voltage detector low-level selection.
  * @param  pConfigVddCOREVM : Pointer to a PWR_VddCOREVMTypeDef structure that contains the
  *                            PVM configuration information (LowVoltageThreshold and Mode).
  * @retval None.
  */
void HAL_PWREx_ConfigVDDCOREVM(const PWR_VddCOREVMTypeDef *pConfigVddCOREVM)
{
  assert_param(IS_PWR_VDDCOREVM_LEVEL(pConfigVddCOREVM->LowVoltageThreshold));
  assert_param(IS_PWR_VDDCOREVM_MODE(pConfigVddCOREVM->Mode));

  /* Disable VDDCORE monitoring Event/Interrupt */
  __HAL_PWR_VCOREVM_EXTI_DISABLE_EVENT();
  __HAL_PWR_VCOREVM_EXTI_DISABLE_IT();
  __HAL_PWR_VCOREVM_EXTI_DISABLE_RISING_EDGE();
  __HAL_PWR_VCOREVM_EXTI_DISABLE_FALLING_EDGE();

  /* Set the VDDCORE voltage detector low-level selection */
  MODIFY_REG(PWR->CR3, PWR_CR3_VCORELLS_Msk, pConfigVddCOREVM->LowVoltageThreshold);

  /* Configure the VDDCORE monitoring in interrupt mode */
  if ((pConfigVddCOREVM->Mode & VDDCOREVM_MODE_IT) == VDDCOREVM_MODE_IT)
  {
    __HAL_PWR_VCOREVM_EXTI_ENABLE_IT();
  }

  /* Configure the VDDCORE monitoring in event mode */
  if ((pConfigVddCOREVM->Mode & VDDCOREVM_MODE_EVT) == VDDCOREVM_MODE_EVT)
  {
    __HAL_PWR_VCOREVM_EXTI_ENABLE_EVENT();
  }

  /* Rising edge configuration */
  if ((pConfigVddCOREVM->Mode & VDDCOREVM_RISING_EDGE) == VDDCOREVM_RISING_EDGE)
  {
    __HAL_PWR_VCOREVM_EXTI_ENABLE_RISING_EDGE();
  }

  /* Falling edge configuration */
  if ((pConfigVddCOREVM->Mode & VDDCOREVM_FALLING_EDGE) == VDDCOREVM_FALLING_EDGE)
  {
    __HAL_PWR_VCOREVM_EXTI_ENABLE_FALLING_EDGE();
  }
}

/**
  * @brief Indicate whether the VDDCORE level is between, above or below
  *        the thresholds.
  * @retval VDDCORE level.
  */
uint32_t HAL_PWREx_GetVDDCORELevel(void)
{
  uint32_t vcoreLevel;
  uint32_t regValue;

  /* Read the VCORE flags */
  regValue = READ_BIT(PWR->CR3, (PWR_CR3_VCOREH | PWR_CR3_VCOREL));

  /* Check if the VCORE is below the threshold */
  if (regValue == PWR_CR3_VCOREL)
  {
    vcoreLevel = PWR_VDDCORE_BELOW_LOW_THRESHOLD;
  }
  /* Check if the VCORE is above the threshold */
  else if (regValue == PWR_CR3_VCOREH)
  {
    vcoreLevel = PWR_VDDCORE_ABOVE_HIGH_THRESHOLD;
  }
  /* The VCORE is between the thresholds */
  else
  {
    vcoreLevel = PWR_VDDCORE_BETWEEN_HIGH_LOW_THRESHOLD;
  }

  return vcoreLevel;
}

/**
  * @brief  Configure the VDD I/O voltage range.
  * @param  VddIOPort VDD I/O
  *         This parameter can be one of the following values:
  *           @arg @ref PWR_VDDIO           Vdd IO
  *           @arg @ref PWR_VDDIO2          Vdd IO2
  *           @arg @ref PWR_VDDIO3          Vdd IO3
  *           @arg @ref PWR_VDDIO4          Vdd IO4
  *           @arg @ref PWR_VDDIO5          Vdd IO5
  * @param  VoltageRange Voltage range
  *         This parameter can be one of the following values:
  *           @arg @ref PWR_VDDIO_RANGE_3V3   3v3 voltage range
  *           @arg @ref PWR_VDDIO_RANGE_1V8   1v8 voltage range
  * @note   HSLV_VDDIOx option bit must be set to allow 1v8 voltage
  *         range operation.
  * @note   Setting this configuration while VDDIOx is in 3v3 range
  *         damages the device.
  * @retval None.
  */
void HAL_PWREx_ConfigVddIORange(uint32_t VddIOPort, uint32_t VoltageRange)
{
  /* Check the parameters */
  assert_param(IS_PWR_VDDIO(VddIOPort));
  assert_param(IS_PWR_VDDIO_RANGE(VoltageRange));

  switch (VddIOPort)
  {
    case PWR_VDDIO:
      MODIFY_REG(PWR->SVMCR3, PWR_SVMCR3_VDDIOVRSEL, VoltageRange << PWR_SVMCR3_VDDIOVRSEL_Pos);
      break;

    case PWR_VDDIO2:
      MODIFY_REG(PWR->SVMCR3, PWR_SVMCR3_VDDIO2VRSEL, VoltageRange << PWR_SVMCR3_VDDIO2VRSEL_Pos);
      break;

    case PWR_VDDIO3:
      MODIFY_REG(PWR->SVMCR3, PWR_SVMCR3_VDDIO3VRSEL, VoltageRange << PWR_SVMCR3_VDDIO3VRSEL_Pos);
      break;

    case PWR_VDDIO4:
      MODIFY_REG(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VRSEL, VoltageRange << PWR_SVMCR1_VDDIO4VRSEL_Pos);
      break;

    case PWR_VDDIO5:
      MODIFY_REG(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VRSEL, VoltageRange << PWR_SVMCR2_VDDIO5VRSEL_Pos);
      break;

    default:
      break;
  }
}

/**
  * @brief  Configure the VDD I/O voltage range.
  * @param  VddIOPort VDD I/O
  *         This parameter can be one of the following values:
  *           @arg @ref PWR_VDDIO           Vdd IO
  *           @arg @ref PWR_VDDIO2          Vdd IO2
  *           @arg @ref PWR_VDDIO3          Vdd IO3
  *           @arg @ref PWR_VDDIO4          Vdd IO4
  *           @arg @ref PWR_VDDIO5          Vdd IO5
  * @retval The voltage range:
  *           @arg @ref PWR_VDDIO_RANGE_3V3   3v3 voltage range
  *           @arg @ref PWR_VDDIO_RANGE_1V8   1v8 voltage range
  */
uint32_t HAL_PWREx_GetVddIORange(uint32_t VddIOPort)
{
  uint32_t voltage_range = 0U;

  /* Check the parameters */
  assert_param(IS_PWR_VDDIO(VddIOPort));

  switch (VddIOPort)
  {
    case PWR_VDDIO:
      voltage_range = (READ_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIOVRSEL) >> PWR_SVMCR3_VDDIOVRSEL_Pos);
      break;

    case PWR_VDDIO2:
      voltage_range = (READ_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2VRSEL) >> PWR_SVMCR3_VDDIO2VRSEL_Pos);
      break;

    case PWR_VDDIO3:
      voltage_range = (READ_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3VRSEL) >> PWR_SVMCR3_VDDIO3VRSEL_Pos);
      break;

    case PWR_VDDIO4:
      voltage_range = (READ_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VRSEL) >> PWR_SVMCR1_VDDIO4VRSEL_Pos);
      break;

    case PWR_VDDIO5:
      voltage_range = (READ_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VRSEL) >> PWR_SVMCR2_VDDIO5VRSEL_Pos);
      break;

    default:
      break;
  }
  return voltage_range;
}

/**
  * @brief  Enable VDD IO4 voltage range Standby mode.
  * @note   When this bit is set, the VDDIO4VRSEL configuration
  *         is retained in Standby mode.
  * @note   This bit must be set if the VDDIO4 is in 1v8 range
  *         in Standby mode, and when exiting Standby mode.
  * @note   It must not be set when VDDIO4 is in 3v3 range in
  *         Standby mode, or when exiting Standby mode.
  * @retval None.
  */
void HAL_PWREx_EnableVddIO4RangeSTBY(void)
{
  SET_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VRSTBY);
}

/**
  * @brief  Disable VDD IO4 voltage range Standby mode.
  * @retval None.
  */
void HAL_PWREx_DisableVddIO4RangeSTBY(void)
{
  CLEAR_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VRSTBY);
}

/**
  * @brief  Enable VDD IO5 voltage range Standby mode.
  * @note   When this bit is set, the VDDIO5VRSEL configuration
  *         is retained in Standby mode.
  * @note   This bit must be set if the VDDIO5 is in 1v8 range
  *         in Standby mode, and when exiting Standby mode.
  * @note   It must not be set when VDDIO5 is in 3v3 range in
  *         Standby mode, or when exiting Standby mode.
  * @retval None.
  */
void HAL_PWREx_EnableVddIO5RangeSTBY(void)
{
  SET_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VRSTBY);
}

/**
  * @brief  Disable VDD IO5 voltage range Standby mode.
  * @retval None.
  */
void HAL_PWREx_DisableVddIO5RangeSTBY(void)
{
  CLEAR_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VRSTBY);
}

/**
  * @brief  Enable VDDUSB supply valid.
  * @note   Setting this bit is mandatory to use the USB2 HS PHYs.
  *         is present for consumption saving.
  * @retval None.
  */
void HAL_PWREx_EnableVddUSB(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_USB33SV);
}

/**
  * @brief  Disable VDDUSB supply valid.
  * @retval None.
  */
void HAL_PWREx_DisableVddUSB(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_USB33SV);
}

/**
  * @brief  Enable VDDIO2 supply valid.
  * @note   Setting this bit is mandatory to use PO[5:0] and PP[15:0] I/Os.
  * @retval None.
  */
void HAL_PWREx_EnableVddIO2(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2SV);
}

/**
  * @brief  Disable VDDIO2 supply valid.
  * @retval None.
  */
void HAL_PWREx_DisableVddIO2(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2SV);
}

/**
  * @brief  Enable VDDIO3 supply valid.
  * @note   Setting this bit is mandatory to use PN[12:0] I/Os.
  * @retval None.
  */
void HAL_PWREx_EnableVddIO3(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3SV);
}

/**
  * @brief  Disable VDDIO3 supply valid.
  * @retval None.
  */
void HAL_PWREx_DisableVddIO3(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3SV);
}

/**
  * @brief  Enable VDDIO4 supply valid.
  * @note   Setting this bit is mandatory to use PB[9,8], PC[12:6], and PD[2] I/Os.
  * @retval None.
  */
void HAL_PWREx_EnableVddIO4(void)
{
  SET_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4SV);
}

/**
  * @brief  Disable VDDIO4 supply valid.
  * @retval None.
  */
void HAL_PWREx_DisableVddIO4(void)
{
  CLEAR_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4SV);
}

/**
  * @brief  Enable VDDIO5 supply valid.
  * @note   Setting this bit is mandatory to use PN[12:0] I/Os.
  * @retval None.
  */
void HAL_PWREx_EnableVddIO5(void)
{
  SET_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5SV);
}

/**
  * @brief  Disable VDDIO5 supply valid.
  * @retval None.
  */
void HAL_PWREx_DisableVddIO5(void)
{
  CLEAR_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5SV);
}

/**
  * @brief  Enable VDDA supply valid.
  * @note   Setting this bit is mandatory to use the analog to digital converters.
  * @retval None.
  */
void HAL_PWREx_EnableVddA(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_ASV);
}

/**
  * @brief  Disable VDDA supply valid.
  * @retval None.
  */
void HAL_PWREx_DisableVddA(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_ASV);
}

/**
  * @brief  Enable the VDD33USB independent USB 33 voltage monitor.
  * @retval None.
  */
void HAL_PWREx_EnableVddUSBVMEN(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_USB33VMEN);
}

/**
  * @brief  Disable the VDD33USB independent USB 33 voltage monitor.
  * @retval None.
  */
void HAL_PWREx_DisableVddUSBVMEN(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_USB33VMEN);
}

/**
  * @brief  Enable the VDDIO2 independent voltage monitor.
  * @retval None.
  */
void HAL_PWREx_EnableVddIO2VMEN(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2VMEN);
}

/**
  * @brief  Disable the VDDIO2 independent voltage monitor.
  * @retval None.
  */
void HAL_PWREx_DisableVddIO2VMEN(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2VMEN);
}

/**
  * @brief  Enable the VDDIO3 independent voltage monitor.
  * @retval None.
  */
void HAL_PWREx_EnableVddIO3VMEN(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3VMEN);
}

/**
  * @brief  Disable the VDDIO3 independent voltage monitor.
  * @retval None.
  */
void HAL_PWREx_DisableVddIO3VMEN(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3VMEN);
}

/**
  * @brief  Enable the VDDIO4 independent voltage monitor.
  * @retval None.
  */
void HAL_PWREx_EnableVddIO4VMEN(void)
{
  SET_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VMEN);
}

/**
  * @brief  Disable the VDDIO4 independent voltage monitor.
  * @retval None.
  */
void HAL_PWREx_DisableVddIO4VMEN(void)
{
  CLEAR_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VMEN);
}

/**
  * @brief  Enable the VDDIO5 independent voltage monitor.
  * @retval None.
  */
void HAL_PWREx_EnableVddIO5VMEN(void)
{
  SET_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VMEN);
}

/**
  * @brief  Disable the VDDIO5 independent voltage monitor.
  * @retval None.
  */
void HAL_PWREx_DisableVddIO5VMEN(void)
{
  CLEAR_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VMEN);
}

/**
  * @brief  Enable the VDDA18ADC independent ADC voltage monitor.
  * @retval None.
  */
void HAL_PWREx_EnableVddAVMEN(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_AVMEN);
}

/**
  * @brief  Disable the VDDA18ADC independent ADC voltage monitor.
  * @retval None.
  */
void HAL_PWREx_DisableVddAVMEN(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_AVMEN);
}

/**
  * @brief  Configure the voltage monitor threshold detected by the Peripheral
  *         voltage monitoring (PVM).
  * @param  pConfigPVM : Pointer to a PWR_PVMTypeDef structure that contains the
  *                      PVM configuration information (PVMType and EventMode).
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_PWREx_ConfigPVM(const PWR_PVMTypeDef *pConfigPVM)
{

  /* Check the parameters */
  assert_param(IS_PWR_PVM_TYPE(pConfigPVM->PVMType));
  assert_param(IS_PWR_PVM_MODE(pConfigPVM->Mode));

  /* Check the peripheral voltage monitor type */
  switch (pConfigPVM->PVMType)
  {
    case PWR_VDDUSB_VM: /* Independent USB voltage monitor */

      /* Disable EXTI USBVM event and interrupt */
      __HAL_PWR_USBVM_EXTI_DISABLE_EVENT();
      __HAL_PWR_USBVM_EXTI_DISABLE_IT();
      __HAL_PWR_USBVM_EXTI_DISABLE_RISING_EDGE();
      __HAL_PWR_USBVM_EXTI_DISABLE_FALLING_EDGE();

      /* Configure the USBVM in interrupt mode */
      if ((pConfigPVM->Mode & PVM_MODE_IT) == PVM_MODE_IT)
      {
        __HAL_PWR_USBVM_EXTI_ENABLE_IT();
      }

      /* Configure the USBVM in event mode */
      if ((pConfigPVM->Mode & PVM_MODE_EVT) == PVM_MODE_EVT)
      {
        __HAL_PWR_USBVM_EXTI_ENABLE_EVENT();
      }

      /* Configure the USBVM in rising edge */
      if ((pConfigPVM->Mode & PVM_RISING_EDGE) == PVM_RISING_EDGE)
      {
        __HAL_PWR_USBVM_EXTI_ENABLE_RISING_EDGE();
      }

      /* Configure the USBVM in falling edge */
      if ((pConfigPVM->Mode & PVM_FALLING_EDGE) == PVM_FALLING_EDGE)
      {
        __HAL_PWR_USBVM_EXTI_ENABLE_FALLING_EDGE();
      }
      break;

    case PWR_VDDIO2_VM: /* Independent I/Os voltage monitor */

      /* Disable EXTI IO2VM event and interrupt */
      __HAL_PWR_IO2VM_EXTI_DISABLE_EVENT();
      __HAL_PWR_IO2VM_EXTI_DISABLE_IT();
      __HAL_PWR_IO2VM_EXTI_DISABLE_RISING_EDGE();
      __HAL_PWR_IO2VM_EXTI_DISABLE_FALLING_EDGE();

      /* Configure the IO2VM in interrupt mode */
      if ((pConfigPVM->Mode & PVM_MODE_IT) == PVM_MODE_IT)
      {
        __HAL_PWR_IO2VM_EXTI_ENABLE_IT();
      }

      /* Configure the IO2VM in event mode */
      if ((pConfigPVM->Mode & PVM_MODE_EVT) == PVM_MODE_EVT)
      {
        __HAL_PWR_IO2VM_EXTI_ENABLE_EVENT();
      }

      /* Configure the IO2VM in rising edge */
      if ((pConfigPVM->Mode & PVM_RISING_EDGE) == PVM_RISING_EDGE)
      {
        __HAL_PWR_IO2VM_EXTI_ENABLE_RISING_EDGE();
      }

      /* Configure the IO2VM in falling edge */
      if ((pConfigPVM->Mode & PVM_FALLING_EDGE) == PVM_FALLING_EDGE)
      {
        __HAL_PWR_IO2VM_EXTI_ENABLE_FALLING_EDGE();
      }
      break;

    case PWR_VDDIO3_VM: /* Independent I/Os voltage monitor */

      /* Disable EXTI IO3VM event and interrupt */
      __HAL_PWR_IO3VM_EXTI_DISABLE_EVENT();
      __HAL_PWR_IO3VM_EXTI_DISABLE_IT();
      __HAL_PWR_IO3VM_EXTI_DISABLE_RISING_EDGE();
      __HAL_PWR_IO3VM_EXTI_DISABLE_FALLING_EDGE();

      /* Configure the IO3VM in interrupt mode */
      if ((pConfigPVM->Mode & PVM_MODE_IT) == PVM_MODE_IT)
      {
        __HAL_PWR_IO3VM_EXTI_ENABLE_IT();
      }

      /* Configure the IO3VM in event mode */
      if ((pConfigPVM->Mode & PVM_MODE_EVT) == PVM_MODE_EVT)
      {
        __HAL_PWR_IO3VM_EXTI_ENABLE_EVENT();
      }

      /* Configure the IO3VM in rising edge */
      if ((pConfigPVM->Mode & PVM_RISING_EDGE) == PVM_RISING_EDGE)
      {
        __HAL_PWR_IO3VM_EXTI_ENABLE_RISING_EDGE();
      }

      /* Configure the IO3VM in falling edge */
      if ((pConfigPVM->Mode & PVM_FALLING_EDGE) == PVM_FALLING_EDGE)
      {
        __HAL_PWR_IO3VM_EXTI_ENABLE_FALLING_EDGE();
      }
      break;

    case PWR_VDDIO4_VM: /* Independent I/Os voltage monitor */

      /* Disable EXTI IO4VM event and interrupt */
      __HAL_PWR_IO4VM_EXTI_DISABLE_EVENT();
      __HAL_PWR_IO4VM_EXTI_DISABLE_IT();
      __HAL_PWR_IO4VM_EXTI_DISABLE_RISING_EDGE();
      __HAL_PWR_IO4VM_EXTI_DISABLE_FALLING_EDGE();

      /* Configure the IO4VM in interrupt mode */
      if ((pConfigPVM->Mode & PVM_MODE_IT) == PVM_MODE_IT)
      {
        __HAL_PWR_IO4VM_EXTI_ENABLE_IT();
      }

      /* Configure the IO4VM in event mode */
      if ((pConfigPVM->Mode & PVM_MODE_EVT) == PVM_MODE_EVT)
      {
        __HAL_PWR_IO4VM_EXTI_ENABLE_EVENT();
      }

      /* Configure the IO4VM in rising edge */
      if ((pConfigPVM->Mode & PVM_RISING_EDGE) == PVM_RISING_EDGE)
      {
        __HAL_PWR_IO4VM_EXTI_ENABLE_RISING_EDGE();
      }

      /* Configure the IO4VM in falling edge */
      if ((pConfigPVM->Mode & PVM_FALLING_EDGE) == PVM_FALLING_EDGE)
      {
        __HAL_PWR_IO4VM_EXTI_ENABLE_FALLING_EDGE();
      }
      break;

    case PWR_VDDIO5_VM: /* Independent I/Os voltage monitor */

      /* Disable EXTI IO5VM event and interrupt */
      __HAL_PWR_IO5VM_EXTI_DISABLE_EVENT();
      __HAL_PWR_IO5VM_EXTI_DISABLE_IT();
      __HAL_PWR_IO5VM_EXTI_DISABLE_RISING_EDGE();
      __HAL_PWR_IO5VM_EXTI_DISABLE_FALLING_EDGE();

      /* Configure the IO5VM in interrupt mode */
      if ((pConfigPVM->Mode & PVM_MODE_IT) == PVM_MODE_IT)
      {
        __HAL_PWR_IO5VM_EXTI_ENABLE_IT();
      }

      /* Configure the IO5VM in event mode */
      if ((pConfigPVM->Mode & PVM_MODE_EVT) == PVM_MODE_EVT)
      {
        __HAL_PWR_IO5VM_EXTI_ENABLE_EVENT();
      }

      /* Configure the IO5VM in rising edge */
      if ((pConfigPVM->Mode & PVM_RISING_EDGE) == PVM_RISING_EDGE)
      {
        __HAL_PWR_IO5VM_EXTI_ENABLE_RISING_EDGE();
      }

      /* Configure the IO5VM in falling edge */
      if ((pConfigPVM->Mode & PVM_FALLING_EDGE) == PVM_FALLING_EDGE)
      {
        __HAL_PWR_IO5VM_EXTI_ENABLE_FALLING_EDGE();
      }
      break;

    case PWR_VDDA_VM: /* Independent ADC voltage monitor */

      /* Disable EXTI ADCVM event and interrupt */
      __HAL_PWR_ADCVM_EXTI_DISABLE_EVENT();
      __HAL_PWR_ADCVM_EXTI_DISABLE_IT();
      __HAL_PWR_ADCVM_EXTI_DISABLE_RISING_EDGE();
      __HAL_PWR_ADCVM_EXTI_DISABLE_FALLING_EDGE();

      /* Configure the ADCVM in interrupt mode */
      if ((pConfigPVM->Mode & PVM_MODE_IT) == PVM_MODE_IT)
      {
        __HAL_PWR_ADCVM_EXTI_ENABLE_IT();
      }

      /* Configure the ADCVM in event mode */
      if ((pConfigPVM->Mode & PVM_MODE_EVT) == PVM_MODE_EVT)
      {
        __HAL_PWR_ADCVM_EXTI_ENABLE_EVENT();
      }

      /* Configure the ADCVM in rising edge */
      if ((pConfigPVM->Mode & PVM_RISING_EDGE) == PVM_RISING_EDGE)
      {
        __HAL_PWR_ADCVM_EXTI_ENABLE_RISING_EDGE();
      }

      /* Configure the ADCVM in falling edge */
      if ((pConfigPVM->Mode & PVM_FALLING_EDGE) == PVM_FALLING_EDGE)
      {
        __HAL_PWR_ADCVM_EXTI_ENABLE_FALLING_EDGE();
      }
      break;

    default: /* No valid voltage monitor selected */
      return HAL_ERROR;
      break;
  }

  return HAL_OK;
}

/**
  * @brief  This function handles the PWR PVD/PVM interrupt request.
  * @retval None.
  */
void HAL_PWREx_PVD_PVM_IRQHandler(void)
{
  uint32_t  rising_flag;
  uint32_t  falling_flag;

  rising_flag  = READ_REG(EXTI->RPR3);
  falling_flag = READ_REG(EXTI->FPR3);

  /* Check PWR VDDCORE monitoring exti rising flag */
  if ((rising_flag & PWR_EXTI_LINE_VCOREVM) != 0U)
  {
    /* Clear VDDCORE monitoring exti pending bit */
    __HAL_PWR_VCOREVM_EXTI_CLEAR_RISING_FLAG();

    /* PWR VDDCORE monitoring interrupt rising user callback */
    HAL_PWREx_VDDCORE_Rising_Callback();
  }

  /* Check PWR VDDCORE monitoring exti falling flag */
  if ((falling_flag & PWR_EXTI_LINE_VCOREVM) != 0U)
  {
    /* Clear VDDCORE monitoring exti pending bit */
    __HAL_PWR_VCOREVM_EXTI_CLEAR_FALLING_FLAG();

    /* PWR VDDCORE monitoring interrupt falling user callback */
    HAL_PWREx_VDDCORE_Falling_Callback();
  }

  /* Check PWR PVD exti rising flag */
  if ((rising_flag & PWR_EXTI_LINE_PVD) != 0U)
  {
    /* Clear PVD exti pending bit */
    __HAL_PWR_PVD_EXTI_CLEAR_RISING_FLAG();

    /* PWR PVD interrupt rising user callback */
    HAL_PWR_PVD_Rising_Callback();
  }

  /* Check PWR PVD exti falling flag */
  if ((falling_flag & PWR_EXTI_LINE_PVD) != 0U)
  {
    /* Clear PVD exti pending bit */
    __HAL_PWR_PVD_EXTI_CLEAR_FALLING_FLAG();

    /* PWR PVD interrupt falling user callback */
    HAL_PWR_PVD_Falling_Callback();
  }

  /* Check PWR PVM USB exti rising flag */
  if ((rising_flag & PWR_EXTI_LINE_PVM_VDDUSB) != 0U)
  {
    /* Clear PVM USB exti pending bit */
    __HAL_PWR_USBVM_EXTI_CLEAR_RISING_FLAG();

    /* PWR PVM USB interrupt rising user callback */
    HAL_PWREx_USBVM_Rising_Callback();
  }

  /* Check PWR PVM USB exti falling flag */
  if ((falling_flag & PWR_EXTI_LINE_PVM_VDDUSB) != 0U)
  {
    /* Clear PVM USB exti pending bit */
    __HAL_PWR_USBVM_EXTI_CLEAR_FALLING_FLAG();

    /* PWR PVM USB interrupt falling user callback */
    HAL_PWREx_USBVM_Falling_Callback();
  }

  /* Check PWR PVM IO2 exti rising flag */
  if ((rising_flag & PWR_EXTI_LINE_PVM_VDDIO2) != 0U)
  {
    /* Clear PVM IO2 exti pending bit */
    __HAL_PWR_IO2VM_EXTI_CLEAR_RISING_FLAG();

    /* PWR PVM IO2 interrupt rising user callback */
    HAL_PWREx_IO2VM_Rising_Callback();
  }

  /* Check PWR PVM IO2 exti falling flag */
  if ((falling_flag & PWR_EXTI_LINE_PVM_VDDIO2) != 0U)
  {
    /* Clear PVM IO2 exti pending bit */
    __HAL_PWR_IO2VM_EXTI_CLEAR_FALLING_FLAG();

    /* PWR PVM IO2 interrupt falling user callback */
    HAL_PWREx_IO2VM_Falling_Callback();
  }

  /* Check PWR PVM IO3 exti rising flag */
  if ((rising_flag & PWR_EXTI_LINE_PVM_VDDIO3) != 0U)
  {
    /* Clear PVM IO3 exti pending bit */
    __HAL_PWR_IO3VM_EXTI_CLEAR_RISING_FLAG();

    /* PWR PVM IO3 interrupt rising user callback */
    HAL_PWREx_IO3VM_Rising_Callback();
  }

  /* Check PWR PVM IO3 exti falling flag */
  if ((falling_flag & PWR_EXTI_LINE_PVM_VDDIO3) != 0U)
  {
    /* Clear PVM IO3 exti pending bit */
    __HAL_PWR_IO3VM_EXTI_CLEAR_FALLING_FLAG();

    /* PWR PVM IO3 interrupt falling user callback */
    HAL_PWREx_IO3VM_Falling_Callback();
  }

  /* Check PWR PVM IO4 exti rising flag */
  if ((rising_flag & PWR_EXTI_LINE_PVM_VDDIO4) != 0U)
  {
    /* Clear PVM IO4 exti pending bit */
    __HAL_PWR_IO4VM_EXTI_CLEAR_RISING_FLAG();

    /* PWR PVM IO4 interrupt rising user callback */
    HAL_PWREx_IO4VM_Rising_Callback();
  }

  /* Check PWR PVM IO4 exti falling flag */
  if ((falling_flag & PWR_EXTI_LINE_PVM_VDDIO4) != 0U)
  {
    /* Clear PVM IO4 exti pending bit */
    __HAL_PWR_IO4VM_EXTI_CLEAR_FALLING_FLAG();

    /* PWR PVM IO4 interrupt falling user callback */
    HAL_PWREx_IO4VM_Falling_Callback();
  }

  /* Check PWR PVM IO5 exti rising flag */
  if ((rising_flag & PWR_EXTI_LINE_PVM_VDDIO5) != 0U)
  {
    /* Clear PVM IO5 exti pending bit */
    __HAL_PWR_IO5VM_EXTI_CLEAR_RISING_FLAG();

    /* PWR PVM IO5 interrupt rising user callback */
    HAL_PWREx_IO5VM_Rising_Callback();
  }

  /* Check PWR PVM IO5 exti falling flag */
  if ((falling_flag & PWR_EXTI_LINE_PVM_VDDIO5) != 0U)
  {
    /* Clear PVM IO5 exti pending bit */
    __HAL_PWR_IO5VM_EXTI_CLEAR_FALLING_FLAG();

    /* PWR PVM IO5 interrupt falling user callback */
    HAL_PWREx_IO5VM_Falling_Callback();

  }

  /* Check PWR PVM ADC VM exti rising flag */
  if ((rising_flag & PWR_EXTI_LINE_PVM_VDDA) != 0U)
  {
    /* Clear PVM ADC VM exti pending bit */
    __HAL_PWR_ADCVM_EXTI_CLEAR_RISING_FLAG();

    /* PWR PVM ADC VM interrupt rising user callback */
    HAL_PWREx_ADCVM_Rising_Callback();
  }

  /* Check PWR PVM ADC VM exti falling flag */
  if ((falling_flag & PWR_EXTI_LINE_PVM_VDDA) != 0U)
  {
    /* Clear PVM ADC VM exti pending bit */
    __HAL_PWR_ADCVM_EXTI_CLEAR_FALLING_FLAG();

    /* PWR PVM ADC VM interrupt falling user callback */
    HAL_PWREx_ADCVM_Falling_Callback();
  }
}

/**
  * @brief  PWR VDDCORE interrupt Rising callback.
  * @retval None.
  */
__weak void HAL_PWREx_VDDCORE_Rising_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_VDDCORE_Rising_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR VDDCORE interrupt Falling callback.
  * @retval None.
  */
__weak void HAL_PWREx_VDDCORE_Falling_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_VDDCORE_Falling_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR USBVM interrupt Rising callback.
  * @retval None.
  */
__weak void HAL_PWREx_USBVM_Rising_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_USBVM_Rising_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR USBVM interrupt Falling callback.
  * @retval None.
  */
__weak void HAL_PWREx_USBVM_Falling_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_USBVM_Falling_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR IO2VM interrupt Rising callback.
  * @retval None.
  */
__weak void HAL_PWREx_IO2VM_Rising_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_IO2VM_Rising_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR IO2VM interrupt Falling callback.
  * @retval None.
  */
__weak void HAL_PWREx_IO2VM_Falling_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_IO2VM_Falling_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR IO3VM interrupt Rising callback.
  * @retval None.
  */
__weak void HAL_PWREx_IO3VM_Rising_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_IO3VM_Rising_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR IO3VM interrupt Falling callback.
  * @retval None.
  */
__weak void HAL_PWREx_IO3VM_Falling_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_IO3VM_Falling_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR IO4VM interrupt Rising callback.
  * @retval None.
  */
__weak void HAL_PWREx_IO4VM_Rising_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_IO4VM_Rising_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR IO4VM interrupt Falling callback.
  * @retval None.
  */
__weak void HAL_PWREx_IO4VM_Falling_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_IO4VM_Falling_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR IO5VM interrupt Rising callback.
  * @retval None.
  */
__weak void HAL_PWREx_IO5VM_Rising_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_IO5VM_Rising_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR IO5VM interrupt Falling callback.
  * @retval None.
  */
__weak void HAL_PWREx_IO5VM_Falling_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_IO5VM_Falling_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR ADCVM interrupt Rising callback.
  * @retval None.
  */
__weak void HAL_PWREx_ADCVM_Rising_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_ADCVM_Rising_Callback() API can be implemented in the user file
   */
}

/**
  * @brief  PWR ADCVM interrupt Falling callback.
  * @retval None.
  */
__weak void HAL_PWREx_ADCVM_Falling_Callback(void)
{
  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_PWREx_ADCVM_Falling_Callback() API can be implemented in the user file
   */
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#endif /* defined (HAL_PWR_MODULE_ENABLED) */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
