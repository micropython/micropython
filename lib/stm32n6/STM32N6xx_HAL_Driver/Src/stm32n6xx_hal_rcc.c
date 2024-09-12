/**
  ******************************************************************************
  * @file    stm32n6xx_hal_rcc.c
  * @author  MCD Application Team
  * @brief   RCC HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Reset and Clock Control (RCC) peripheral:
  *           + Initialization and de-initialization functions
  *           + Peripheral Control functions
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                      ##### RCC specific features #####
  ==============================================================================
    [..]
      After reset the device is running from Internal High Speed oscillator
      (HSI 64MHz) and all peripherals are off except internal SRAM, BSEC, PWR, IAC,
      RIFSC, RISAF and JTAG
      (+) There is no pre-scaler on High speed (AHB) and Low speed (APB) buses;
          all peripherals mapped on these buses are running at HSI speed.
      (+) All GPIOs are in analog mode , except the JTAG pins which
          are assigned to be used for debug purpose.

    [..]
      Once the device started from reset, the user application has to:
      (+) Configure the clock source to be used to drive the System clock
          (if the application needs higher frequency/performance than HSI)
      (+) Configure the CPU and System bus clock frequencies
      (+) Configure the AHB and APB buses pre-scalers
      (+) Enable the clock for the peripheral(s) to be used
      (+) Configure the clock kernel source(s) for peripherals which clocks are not
          derived from the System clock

                      ##### RCC Limitations #####
  ==============================================================================
    [..]
      A delay between an RCC peripheral clock enable and the effective peripheral
      enabling should be taken into account in order to manage the peripheral read/write
      from/to registers.
      (+) This delay depends on the peripheral mapping.
      (+) If peripheral is mapped on AHB: the delay is 2 AHB clock cycle
          after the clock enable bit is set on the hardware register
      (+) If peripheral is mapped on APB: the delay is 2 APB clock cycle
          after the clock enable bit is set on the hardware register

    [..]
      Implemented Workaround:
      (+) For AHB & APB peripherals, a dummy read to the peripheral register has been
          inserted in each __HAL_RCC_PPP_CLK_ENABLE() macro.

    [..]
      Incorrect APB prescaler setting:
      (+) As described in the product errata, after enabling the bus clock to the
      peripheral and changing the default APB prescaler setting, performing a
      configuration on the bus peripheral does not update the peripheral registers,
      resulting in unexpected behavior.

  @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup RCC  RCC
  * @brief RCC HAL module driver
  * @{
  */

#ifdef HAL_RCC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/** @defgroup RCC_Private_Constants RCC Private Constants
  * @{
  */
#define RCC_MSI_TIMEOUT_VALUE         1U    /* 1 ms */
#define RCC_HSI_TIMEOUT_VALUE         1U    /* 1 ms */
#define RCC_HSE_TIMEOUT_VALUE         HSE_STARTUP_TIMEOUT
#define RCC_LSI_TIMEOUT_VALUE         1U    /* 1 ms */
#define RCC_CLOCKSWITCH_TIMEOUT_VALUE 5000U /* 5 s  */

#define RCC_PLL1_CONFIG           0U
#define RCC_PLL2_CONFIG           1U
#define RCC_PLL3_CONFIG           2U
#define RCC_PLL4_CONFIG           3U

#define RCC_ITEM_GROUP_IDX_OSC        0x0UL
#define RCC_ITEM_GROUP_IDX_PLL        0x1UL
#define RCC_ITEM_GROUP_IDX_IC         0x2UL
#define RCC_ITEM_GROUP_IDX_SYSCFG     0x3UL
#define RCC_ITEM_GROUP_IDX_BUS        0x4UL
#define RCC_ITEM_GROUP_IDX_MEM        0x5UL
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup RCC_Private_Macros RCC Private Macros
  * @{
  */
#define RCC_MCO1_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define RCC_MCO1_GPIO_PORT        GPIOA
#define RCC_MCO1_PIN              GPIO_PIN_8

#define RCC_MCO2_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define RCC_MCO2_GPIO_PORT        GPIOC
#define RCC_MCO2_PIN              GPIO_PIN_9
/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef RCC_PLL_Config(uint32_t PLLnumber, const RCC_PLLInitTypeDef *pPLLInit);
static HAL_StatusTypeDef RCC_PLL_Enable(uint32_t PLLnumber);
static uint32_t RCC_PLL_IsNewConfig(uint32_t PLLnumber, const RCC_PLLInitTypeDef *pPLLInit);
static uint32_t RCC_PLL_Source_IsReady(uint32_t PLLSource);
static uint32_t RCC_IC_CheckPLLSources(uint32_t PLLSource1, uint32_t PLLSource2);
static void RCC_ATTR_ConfigItemGroup(uint32_t ItemGroup, uint32_t Item, uint32_t Attributes);

/* Exported functions --------------------------------------------------------*/
/** @defgroup RCC_Exported_Functions RCC Exported Functions
  * @{
  */

/** @defgroup RCC_Exported_Functions_Group1 Initialization and de-initialization functions
  *  @brief    Initialization and Configuration functions
  *
@verbatim
 ===============================================================================
           ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]
      This section provides functions allowing to configure the internal/external oscillators
      (HSE, HSI, LSE, MSI, LSI, PLL, CSS and MCO) and the CPU and System buses clocks (SYSCLK, AHB1,
       AHB2, AHB3, AHB4, AHB5, APB1, APB2, APB4 and APB5).

    [..] Internal/external clock and PLL configuration
         (#) HSI (high-speed internal), 64 MHz factory-trimmed RC used directly or through
             the PLL as System clock source.

         (#) MSI is a low-power RC oscillator which can be used directly as system clock, peripheral
             clock, or PLL input.But even with frequency calibration, is less accurate than an
             external crystal oscillator or ceramic resonator.

         (#) LSI (low-speed internal), 32 KHz low consumption RC used as IWDG and/or RTC
             clock source.

         (#) HSE (high-speed external), 4 to 48 MHz crystal oscillator used directly or
             through the PLL as System clock source. Can be used also as RTC clock source.

         (#) LSE (low-speed external), 32 KHz oscillator used as RTC clock source.

         (#) PLL , The RCC features three independent PLLs (clocked by HSI , HSE or MSI),
             featuring three different output clocks and able  to work either in integer or Fractional mode.
           (++) A main PLL, PLL1, which is generally used to provide clocks to the CPU
                and to some peripherals via the ICx mux.
           (++) Two dedicated PLLs, PLL2 and PLL3, which are used to generate the kernel clock for peripherals.

         (#) HSE CSS (Clock security system), once enabled and if a HSE clock failure occurs
            (HSE used directly or through PLL as System clock source), the System clock
             is automatically switched to HSI and an interrupt is generated if enabled.
             The interrupt is linked to the Cortex-M NMI (Non-Maskable Interrupt)
             exception vector.

         (#) LSE CSS (Clock security system), once enabled and if a LSE clock failure occurs
             the LSE is no more provided to RTC, it allows to wake-up from standby mode and protect the
             backup registers and BKPSRAM via TAMP. The LSE CSS may be re-armed for further attempt(s) to use LSE.

         (#) MCO1 (micro controller clock output), used to output HSI, LSE, HSE, LSI, MSI, CPU (SYSA), IC5 mux
             or IC10 mux clock (through a configurable pre-scaler) on PA8 pin.

         (#) MCO2 (micro controller clock output), used to output HSI, LSE, HSE, LSI, MSI, System Bus (SYSB),
             IC15 mux or IC20 mux clock (through a configurable pre-scaler) on PC9 pin.

    [..] CPU, System bus, AHB and APB buses clocks configuration
         (#) Several clock sources can be used to drive independently the CPU (CPUCLK) and System bus clock (SYSCLK):
             HSI, MSI, HSE or ICx mux.
             The AHB clock (HCLK) is derived from System core clock through configurable
             pre-scaler and used to clock the CPU, memory and peripherals mapped
             on AHB and APB bus through configurable pre-scalers
             and used to clock the peripherals mapped on these buses. You can use
             "HAL_RCC_GetSysClockFreq()" function to retrieve system clock frequency.

         -@- All the peripheral clocks are derived from the System bus clock (SYSCLK).

@endverbatim
  * @{
  */

/**
  * @brief  Resets the RCC clock configuration to the default reset state.
  * @note   The default reset state of the clock configuration is given below:
  *            - HSI ON and used as CPU and system clock source
  *            - MSI and HSE OFF
  *            - PLL1, PLL2, PLL3 and PLL4 bypassed
  *            - AHB, APB Bus prescalers set to 1
  *            - CSS, MCO1 and MCO2 OFF
  *            - All interrupts disabled
  * @note   This function doesn't modify the configuration of the
  *            - Peripheral clocks
  *            - Memory clocks
  *            - Misc clocks
  *            - LSI, LSE and RTC clocks
  *            - ICx selections and dividers
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RCC_DeInit(void)
{
  uint32_t tickstart;

  /* Disable all interrupts except default one */
  WRITE_REG(RCC->CIER, RCC_CIER_HSECSSIE);

  /* Clear all interrupts flags */
  WRITE_REG(RCC->CICR, RCC_CICR_LSIRDYC | RCC_CICR_LSERDYC | RCC_CICR_MSIRDYC | RCC_CICR_HSIRDYC | RCC_CICR_HSERDYC | \
            RCC_CICR_PLL1RDYC | RCC_CICR_PLL2RDYC | RCC_CICR_PLL3RDYC | RCC_CICR_PLL4RDYC | \
            RCC_CICR_LSECSSC | RCC_CICR_HSECSSC | RCC_CICR_WKUPFC);

  /* Clear reset source flags */
  SET_BIT(RCC->RSR, RCC_RSR_RMVF);

  /* Restore default HSI */
  LL_RCC_HSI_SetDivider(LL_RCC_HSI_DIV_1);
  LL_RCC_HSI_Enable();

  /* Get Start Tick */
  tickstart = HAL_GetTick();

  /* Wait for HSI READY bit */
  while (LL_RCC_HSI_IsReady() == 0U)
  {
    if ((HAL_GetTick() - tickstart) > RCC_HSI_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Reset CFGR1 register to select HSI as CPU and system bus clock */
  CLEAR_REG(RCC->CFGR1);

  /* Get Start Tick */
  tickstart = HAL_GetTick();

  /* Wait till clock switch is ready */
  while (READ_BIT(RCC->CFGR1, (RCC_CFGR1_CPUSWS | RCC_CFGR1_SYSSWS)) != 0U)
  {
    if ((HAL_GetTick() - tickstart) > RCC_CLOCKSWITCH_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Update the SystemCoreClock variable */
  SystemCoreClock = HSI_VALUE;

  /* Adapt Systick interrupt period */
  if (HAL_InitTick(uwTickPrio) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  /* Reset PLL1 registers to default value */
  WRITE_REG(RCC->PLL1CFGR1, 0x08202500U);
  WRITE_REG(RCC->PLL1CFGR2, 0x00800000U);
  WRITE_REG(RCC->PLL1CFGR3, 0x4900000DU);
  /* Reset PLL2 registers to default value */
  WRITE_REG(RCC->PLL2CFGR1, 0x08000000U);
  WRITE_REG(RCC->PLL2CFGR2, 0x00000000U);
  WRITE_REG(RCC->PLL2CFGR3, 0x49000005U);
  /* Reset PLL3 registers to default value */
  WRITE_REG(RCC->PLL3CFGR1, 0x08000000U);
  WRITE_REG(RCC->PLL3CFGR2, 0x00000000U);
  WRITE_REG(RCC->PLL3CFGR3, 0x49000005U);
  /* Reset PLL4 registers to default value */
  WRITE_REG(RCC->PLL4CFGR1, 0x08000000U);
  WRITE_REG(RCC->PLL4CFGR2, 0x00000000U);
  WRITE_REG(RCC->PLL4CFGR3, 0x49000005U);

  /* Reset MSION, HSEON, PLL1ON, PLL2ON, PLL3ON and PLL4ON bits */
  WRITE_REG(RCC->CCR, RCC_CCR_MSIONC | RCC_CCR_HSEONC | \
            RCC_CCR_PLL1ONC | RCC_CCR_PLL2ONC | RCC_CCR_PLL3ONC | RCC_CCR_PLL4ONC);

  /* Get Start Tick */
  tickstart = HAL_GetTick();

  /* Wait for all PLL ready bits to be reset */
  while (READ_BIT(RCC->SR, (RCC_SR_PLL1RDY | RCC_SR_PLL2RDY | RCC_SR_PLL3RDY | RCC_SR_PLL4RDY)) != 0U)
  {
    if ((HAL_GetTick() - tickstart) > RCC_PLL_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Reset prescalers in CFGR2 register to default values*/
  WRITE_REG(RCC->CFGR2, 0x00100000U);

  /* Reset MSICFGR register */
  CLEAR_REG(RCC->MSICFGR);

  /* Reset HSIMCR register to default value */
  WRITE_REG(RCC->HSIMCR, 0x001F07A1U);

  /* Reset HSECFGR register to default value */
  WRITE_REG(RCC->HSECFGR, 0x00000800U);

  /* Reset STOPCR register to default value */
  WRITE_REG(RCC->STOPCR, 0x00000002U);

  /* Disable MCO1 and MCO2 */
  CLEAR_BIT(RCC->MISCENR, RCC_MISCENR_MCO1EN | RCC_MISCENR_MCO2EN);

  return HAL_OK;
}

/**
  * @brief  Initializes the RCC Oscillators according to the specified parameters in the
  *         RCC_OscInitTypeDef.
  * @param  pRCC_OscInitStruct  Pointer to an RCC_OscInitTypeDef structure that
  *         contains the configuration information for the RCC Oscillators.
  * @note   LSE control requires that the backup domain shall be previously enabled
  *         with HAL_PWR_EnableBkUpAccess().
  * @note   The PLL is not disabled when used as system clock.
  * @note   Transitions LSE Bypass to LSE On and LSE On to LSE Bypass are not
  *         supported by this function. User should request a transition to LSE Off
  *         first and then LSE On or LSE Bypass.
  * @note   Transition HSE Bypass to HSE On and HSE On to HSE Bypass are not
  *         supported by this function. User should request a transition to HSE Off
  *         first and then HSE On or HSE Bypass.
  * @note   This function does not protect the MCOxSEL, the PERSEL and the PPPSEL glitch-free muxes
  *         (Mux selection cannot be changed if selected input clock is inactive).
  * @note   This function activates HSE but does not wait for the startup time defined in the datasheet.
  *         This must be ensured by the application when the HSE is selected as PLL source.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RCC_OscConfig(const RCC_OscInitTypeDef  *pRCC_OscInitStruct)
{
  uint32_t tickstart;
  uint32_t sysclksrc;
  uint32_t cpuclksrc;
  uint32_t pll1src;
  uint32_t pll2src;
  uint32_t pll3src;
  uint32_t pll4src;
  uint32_t rccsr;

  /* Check Null pointer */
  if (pRCC_OscInitStruct == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_RCC_OSCILLATORTYPE(pRCC_OscInitStruct->OscillatorType));

  cpuclksrc = LL_RCC_GetCpuClkSource();
  sysclksrc = LL_RCC_GetSysClkSource();
  pll1src = LL_RCC_PLL1_GetSource();
  pll2src = LL_RCC_PLL2_GetSource();
  pll3src = LL_RCC_PLL3_GetSource();
  pll4src = LL_RCC_PLL4_GetSource();
  rccsr = RCC->SR;

  /*------------------------------- HSE Configuration ------------------------*/
  if (((pRCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSE) == RCC_OSCILLATORTYPE_HSE)
  {
    /* Check the parameters */
    assert_param(IS_RCC_HSE(pRCC_OscInitStruct->HSEState));

    /* When the HSE is used as cpu/system bus clock or clock source for any PLL, it is not allowed to be disabled */
    if ((cpuclksrc == RCC_CPUCLKSOURCE_STATUS_HSE) || (sysclksrc == RCC_SYSCLKSOURCE_STATUS_HSE) ||
        ((pll1src == LL_RCC_PLLSOURCE_HSE) && (((rccsr & RCC_SR_PLL1RDY) == RCC_SR_PLL1RDY))) ||
        ((pll2src == LL_RCC_PLLSOURCE_HSE) && (((rccsr & RCC_SR_PLL2RDY) == RCC_SR_PLL2RDY))) ||
        ((pll3src == LL_RCC_PLLSOURCE_HSE) && (((rccsr & RCC_SR_PLL3RDY) == RCC_SR_PLL3RDY))) ||
        ((pll4src == LL_RCC_PLLSOURCE_HSE) && (((rccsr & RCC_SR_PLL4RDY) == RCC_SR_PLL4RDY))))
    {
      if (pRCC_OscInitStruct->HSEState == RCC_HSE_OFF)
      {
        return HAL_ERROR;
      }
      /* HSE ON , nothing to do */
    }
    else
    {
      /* Set the new HSE configuration ---------------------------------------*/
      __HAL_RCC_HSE_CONFIG(pRCC_OscInitStruct->HSEState);

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Check the HSE State */
      if (pRCC_OscInitStruct->HSEState != RCC_HSE_OFF)
      {
        /* Wait till HSE is ready */
        while (READ_BIT(RCC->SR, RCC_SR_HSERDY) == 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_HSE_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
      else
      {
        /* Wait till HSE is disabled */
        while (READ_BIT(RCC->SR, RCC_SR_HSERDY) != 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_HSE_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
  }

  /*----------------------------- HSI Configuration --------------------------*/
  if (((pRCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI)
  {
    /* Check the parameters */
    assert_param(IS_RCC_HSI(pRCC_OscInitStruct->HSIState));

    /* When the HSI is used as cpu/system bus clock or clock source for any PLL, it is not allowed to be disabled */
    if ((cpuclksrc == RCC_CPUCLKSOURCE_STATUS_HSI) || (sysclksrc == RCC_SYSCLKSOURCE_STATUS_HSI) ||
        ((pll1src == LL_RCC_PLLSOURCE_HSI) && (((rccsr & RCC_SR_PLL1RDY) == RCC_SR_PLL1RDY))) ||
        ((pll2src == LL_RCC_PLLSOURCE_HSI) && (((rccsr & RCC_SR_PLL2RDY) == RCC_SR_PLL2RDY))) ||
        ((pll3src == LL_RCC_PLLSOURCE_HSI) && (((rccsr & RCC_SR_PLL3RDY) == RCC_SR_PLL3RDY))) ||
        ((pll4src == LL_RCC_PLLSOURCE_HSI) && (((rccsr & RCC_SR_PLL4RDY) == RCC_SR_PLL4RDY))))
    {
      /* When HSI is used as system clock it will not be disabled */
      if (pRCC_OscInitStruct->HSIState == RCC_HSI_OFF)
      {
        return HAL_ERROR;
      }
      /* Otherwise, just the divider and calibration is allowed */
      else
      {
        /* Check the parameters */
        assert_param(IS_RCC_HSI_DIV(pRCC_OscInitStruct->HSIDiv));
        assert_param(IS_RCC_HSI_CALIBRATION_VALUE(pRCC_OscInitStruct->HSICalibrationValue));

        /* Set the HSI Divider */
        __HAL_RCC_HSI_DIVIDER_CONFIG(pRCC_OscInitStruct->HSIDiv);
        /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
        __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(pRCC_OscInitStruct->HSICalibrationValue);
      }
    }
    else
    {
      /* Check the HSI State */
      if (pRCC_OscInitStruct->HSIState != RCC_HSI_OFF)
      {
        /* Check the parameters */
        assert_param(IS_RCC_HSI_DIV(pRCC_OscInitStruct->HSIDiv));
        assert_param(IS_RCC_HSI_CALIBRATION_VALUE(pRCC_OscInitStruct->HSICalibrationValue));

        /* Enable the Internal High Speed oscillator (HSI). */
        __HAL_RCC_HSI_ENABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till HSI is ready */
        while (LL_RCC_HSI_IsReady() == 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_HSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }

        /* Set the HSI Divider */
        __HAL_RCC_HSI_DIVIDER_CONFIG(pRCC_OscInitStruct->HSIDiv);
        /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
        __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(pRCC_OscInitStruct->HSICalibrationValue);
      }
      else
      {
        /* Disable the Internal High Speed oscillator (HSI). */
        __HAL_RCC_HSI_DISABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till HSI is disabled */
        while (LL_RCC_HSI_IsReady() != 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_HSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
  }

  /*----------------------------- MSI Configuration --------------------------*/
  if (((pRCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_MSI) == RCC_OSCILLATORTYPE_MSI)
  {
    /* Check the parameters */
    assert_param(IS_RCC_MSI(pRCC_OscInitStruct->MSIState));

    /* When the MSI is used as cpu/system bus clock or clock source for any PLL, it is not allowed to be disabled */
    /* but just to update the MSI calibration value */
    if ((cpuclksrc == RCC_CPUCLKSOURCE_STATUS_MSI) || (sysclksrc == RCC_SYSCLKSOURCE_STATUS_MSI) ||
        ((pll1src == LL_RCC_PLLSOURCE_MSI) && (((rccsr & RCC_SR_PLL1RDY) == RCC_SR_PLL1RDY))) ||
        ((pll2src == LL_RCC_PLLSOURCE_MSI) && (((rccsr & RCC_SR_PLL2RDY) == RCC_SR_PLL2RDY))) ||
        ((pll3src == LL_RCC_PLLSOURCE_MSI) && (((rccsr & RCC_SR_PLL3RDY) == RCC_SR_PLL3RDY))) ||
        ((pll4src == LL_RCC_PLLSOURCE_MSI) && (((rccsr & RCC_SR_PLL4RDY) == RCC_SR_PLL4RDY))))
    {
      /* When MSI is used as system clock it will not disabled */
      if (pRCC_OscInitStruct->MSIState == RCC_MSI_OFF)
      {
        return HAL_ERROR;
      }
      /* Otherwise, just the calibration is allowed */
      else
      {
        /* Check the parameters */
        assert_param(IS_RCC_MSI_CALIBRATION_VALUE(pRCC_OscInitStruct->MSICalibrationValue));

        /* Adjusts the Internal High Speed oscillator (MSI) calibration value.*/
        __HAL_RCC_MSI_CALIBRATIONVALUE_ADJUST(pRCC_OscInitStruct->MSICalibrationValue);
      }
    }
    else
    {
      /* Check the MSI State */
      if ((pRCC_OscInitStruct->MSIState) != RCC_MSI_OFF)
      {
        /* Check the parameters */
        assert_param(IS_RCC_MSI_FREQUENCY(pRCC_OscInitStruct->MSIFrequency));
        assert_param(IS_RCC_MSI_CALIBRATION_VALUE(pRCC_OscInitStruct->MSICalibrationValue));

        /* Set the frequency */
        __HAL_RCC_MSI_FREQUENCY_CONFIG(pRCC_OscInitStruct->MSIFrequency);

        /* Enable the Internal High Speed oscillator (MSI). */
        __HAL_RCC_MSI_ENABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till MSI is ready */
        while (LL_RCC_MSI_IsReady() == 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_MSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }

        /* Adjusts the Internal High Speed oscillator (MSI) calibration value.*/
        __HAL_RCC_MSI_CALIBRATIONVALUE_ADJUST(pRCC_OscInitStruct->MSICalibrationValue);
      }
      else
      {
        /* Ignore MSI frequency and calibration values in disable case */
        /* Disable the Internal High Speed oscillator (MSI). */
        __HAL_RCC_MSI_DISABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till MSI is disabled */
        while (LL_RCC_MSI_IsReady() != 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_MSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
  }

  /*------------------------------ LSI Configuration -------------------------*/
  if (((pRCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LSI(pRCC_OscInitStruct->LSIState));

    /* Check the LSI State */
    if ((pRCC_OscInitStruct->LSIState) != RCC_LSI_OFF)
    {
      /* Enable the Internal Low Speed oscillator (LSI). */
      __HAL_RCC_LSI_ENABLE();

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSI is ready */
      while (LL_RCC_LSI_IsReady() == 0U)
      {
        if ((HAL_GetTick() - tickstart) > RCC_LSI_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else
    {
      /* Disable the Internal Low Speed oscillator (LSI). */
      __HAL_RCC_LSI_DISABLE();

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSI is ready */
      while (LL_RCC_LSI_IsReady() != 0U)
      {
        if ((HAL_GetTick() - tickstart) > RCC_LSI_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
  }

  /*------------------------------ LSE Configuration -------------------------*/
  if (((pRCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LSE(pRCC_OscInitStruct->LSEState));

    /* Set the new LSE configuration -----------------------------------------*/
    __HAL_RCC_LSE_CONFIG(pRCC_OscInitStruct->LSEState);
    /* Check the LSE State */
    if ((pRCC_OscInitStruct->LSEState) != RCC_LSE_OFF)
    {
      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSE is ready */
      while (LL_RCC_LSE_IsReady() == 0U)
      {
        if ((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else
    {
      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSE is disabled */
      while (LL_RCC_LSE_IsReady() != 0U)
      {
        if ((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
  }

  /*-------------------------------- PLL1 Configuration ----------------------*/
  /* Check the parameters */
  assert_param(IS_RCC_PLL(pRCC_OscInitStruct->PLL1.PLLState));

  if (pRCC_OscInitStruct->PLL1.PLLState != RCC_PLL_NONE)
  {
    uint32_t new_pll_config = RCC_PLL_IsNewConfig(RCC_PLL1_CONFIG, &(pRCC_OscInitStruct->PLL1));
    uint32_t pll1_ready = LL_RCC_PLL1_IsReady();
    if (new_pll_config == 1U)
    {
      uint32_t ic1src = LL_RCC_IC1_GetSource();
      uint32_t ic2src = LL_RCC_IC2_GetSource();
      uint32_t ic6src = LL_RCC_IC6_GetSource();
      uint32_t ic11src = LL_RCC_IC11_GetSource();
      /* PLL1 should not be disabled / reconfigured if used for IC1 (cpuclksrc) - return HAL_ERROR */
      if ((cpuclksrc == RCC_CPUCLKSOURCE_STATUS_IC1) && (ic1src == LL_RCC_ICCLKSOURCE_PLL1))
      {
        return HAL_ERROR;
      }

      /* PLL1 should not be disabled / reconfigured if used for IC2, IC6 or IC11 (sysclksrc) - return HAL_ERROR  */
      if ((sysclksrc == RCC_SYSCLKSOURCE_STATUS_IC2_IC6_IC11) && ((ic2src == LL_RCC_ICCLKSOURCE_PLL1) ||
                                                                  (ic6src == LL_RCC_ICCLKSOURCE_PLL1) ||
                                                                  (ic11src == LL_RCC_ICCLKSOURCE_PLL1)))
      {
        return HAL_ERROR;
      }
      /* PLL1 is not used, it can be configured */
      if (RCC_PLL_Config(RCC_PLL1_CONFIG, &(pRCC_OscInitStruct->PLL1)) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }
    else if ((pRCC_OscInitStruct->PLL1.PLLState == RCC_PLL_ON) && (pll1_ready == 0U))
    {
      if (RCC_PLL_Enable(RCC_PLL1_CONFIG) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }
    else
    {
      /* Nothing to do */
    }
  }

  /*-------------------------------- PLL2 Configuration ----------------------*/
  /* Check the parameters */
  assert_param(IS_RCC_PLL(pRCC_OscInitStruct->PLL2.PLLState));

  if (pRCC_OscInitStruct->PLL2.PLLState != RCC_PLL_NONE)
  {
    uint32_t new_pll_config = RCC_PLL_IsNewConfig(RCC_PLL2_CONFIG, &(pRCC_OscInitStruct->PLL2));
    uint32_t pll2_ready = LL_RCC_PLL2_IsReady();
    if (new_pll_config == 1U)
    {
      uint32_t ic1src = LL_RCC_IC1_GetSource();
      uint32_t ic2src = LL_RCC_IC2_GetSource();
      uint32_t ic6src = LL_RCC_IC6_GetSource();
      uint32_t ic11src = LL_RCC_IC11_GetSource();
      /* PLL2 should not be disabled / reconfigured if used for IC1 (cpuclksrc) - return HAL_ERROR */
      if ((cpuclksrc == RCC_CPUCLKSOURCE_STATUS_IC1) && (ic1src == LL_RCC_ICCLKSOURCE_PLL2))
      {
        return HAL_ERROR;
      }

      /* PLL2 should not be disabled / reconfigured if used for IC2, IC6 or IC11 (sysclksrc) - return HAL_ERROR  */
      if ((sysclksrc == RCC_SYSCLKSOURCE_STATUS_IC2_IC6_IC11) && ((ic2src == LL_RCC_ICCLKSOURCE_PLL2) ||
                                                                  (ic6src == LL_RCC_ICCLKSOURCE_PLL2) ||
                                                                  (ic11src == LL_RCC_ICCLKSOURCE_PLL2)))
      {
        return HAL_ERROR;
      }
      /* PLL2 is not used, it can be configured */
      if (RCC_PLL_Config(RCC_PLL2_CONFIG, &(pRCC_OscInitStruct->PLL2)) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }
    else if ((pRCC_OscInitStruct->PLL2.PLLState == RCC_PLL_ON) && (pll2_ready == 0U))
    {
      if (RCC_PLL_Enable(RCC_PLL2_CONFIG) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }
    else
    {
      /* Nothing to do */
    }
  }

  /*-------------------------------- PLL3 Configuration ----------------------*/
  /* Check the parameters */
  assert_param(IS_RCC_PLL(pRCC_OscInitStruct->PLL3.PLLState));

  if (pRCC_OscInitStruct->PLL3.PLLState != RCC_PLL_NONE)
  {
    uint32_t new_pll_config = RCC_PLL_IsNewConfig(RCC_PLL3_CONFIG, &(pRCC_OscInitStruct->PLL3));
    uint32_t pll3_ready = LL_RCC_PLL1_IsReady();
    if (new_pll_config == 1U)
    {
      uint32_t ic1src = LL_RCC_IC1_GetSource();
      uint32_t ic2src = LL_RCC_IC2_GetSource();
      uint32_t ic6src = LL_RCC_IC6_GetSource();
      uint32_t ic11src = LL_RCC_IC11_GetSource();
      /* PLL3 should not be disabled / reconfigured if used for IC1 (cpuclksrc) - return HAL_ERROR */
      if ((cpuclksrc == RCC_CPUCLKSOURCE_STATUS_IC1) && (ic1src == LL_RCC_ICCLKSOURCE_PLL3))
      {
        return HAL_ERROR;
      }
      /* PLL3 should not be disabled / reconfigured if used for IC2, IC6 or IC11 (sysclksrc) - return HAL_ERROR  */
      if ((sysclksrc == RCC_SYSCLKSOURCE_STATUS_IC2_IC6_IC11) && ((ic2src == LL_RCC_ICCLKSOURCE_PLL3) ||
                                                                  (ic6src == LL_RCC_ICCLKSOURCE_PLL3) ||
                                                                  (ic11src == LL_RCC_ICCLKSOURCE_PLL3)))
      {
        return HAL_ERROR;
      }
      /* PLL3 is not used, it can be configured */
      if (RCC_PLL_Config(RCC_PLL3_CONFIG, &(pRCC_OscInitStruct->PLL3)) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }
    else if ((pRCC_OscInitStruct->PLL3.PLLState == RCC_PLL_ON) && (pll3_ready == 0U))
    {
      if (RCC_PLL_Enable(RCC_PLL3_CONFIG) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }
    else
    {
      /* Nothing to do */
    }
  }

  /*-------------------------------- PLL4 Configuration ----------------------*/
  /* Check the parameters */
  assert_param(IS_RCC_PLL(pRCC_OscInitStruct->PLL4.PLLState));

  if (pRCC_OscInitStruct->PLL4.PLLState != RCC_PLL_NONE)
  {
    uint32_t new_pll_config = RCC_PLL_IsNewConfig(RCC_PLL4_CONFIG, &(pRCC_OscInitStruct->PLL4));
    uint32_t pll4_ready = LL_RCC_PLL4_IsReady();

    if (new_pll_config == 1U)
    {
      uint32_t ic1src = LL_RCC_IC1_GetSource();
      uint32_t ic2src = LL_RCC_IC2_GetSource();
      uint32_t ic6src = LL_RCC_IC6_GetSource();
      uint32_t ic11src = LL_RCC_IC11_GetSource();
      /* PLL4 should not be disabled / reconfigured if used for IC1 (cpuclksrc) - return HAL_ERROR */
      if ((cpuclksrc == RCC_CPUCLKSOURCE_STATUS_IC1) && (ic1src == LL_RCC_ICCLKSOURCE_PLL4))
      {
        return HAL_ERROR;
      }
      /* PLL4 should not be disabled / reconfigured if used for IC2, IC6 or IC11 (sysclksrc) - return HAL_ERROR  */
      if ((sysclksrc == RCC_SYSCLKSOURCE_STATUS_IC2_IC6_IC11) && ((ic2src == LL_RCC_ICCLKSOURCE_PLL4) ||
                                                                  (ic6src == LL_RCC_ICCLKSOURCE_PLL4) ||
                                                                  (ic11src == LL_RCC_ICCLKSOURCE_PLL4)))
      {
        return HAL_ERROR;
      }
      /* PLL4 is not used, it can be configured */
      if (RCC_PLL_Config(RCC_PLL4_CONFIG, &(pRCC_OscInitStruct->PLL4)) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }
    else if ((pRCC_OscInitStruct->PLL4.PLLState == RCC_PLL_ON) && (pll4_ready == 0U))
    {
      if (RCC_PLL_Enable(RCC_PLL4_CONFIG) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }
    else
    {
      /* Nothing to do */
    }
  }

  return HAL_OK;
}

/**
  * @brief  Initializes the CPU, System AXI, AHB, APB buses clocks according to the specified
  *         parameters in the RCC_ClkInitStruct.
  * @param  pRCC_ClkInitStruct  Pointer to an RCC_OscInitTypeDef structure that
  *         contains the configuration information for the RCC peripheral.
  *
  * @note   The SystemCoreClock CMSIS variable is used to store System Core Clock Frequency
  *         and updated by HAL_InitTick() function called within this function.
  *
  * @note   The HSI is used (enabled by hardware) as system clock source after
  *         start-up from Reset, wake-up from STOP (unless MSI is selected) and STANDBY mode,
  *         or in case of failure of the HSE used directly or indirectly as system clock
  *         (if the Clock Security System CSS is enabled).
  *
  * @note   A switch from one clock source to another occurs only if the target
  *         clock source is ready (clock stable after start-up delay or PLL locked).
  *         If a clock source which is not yet ready is selected, the switch will
  *         occur when the clock source will be ready.
  *         You can use HAL_RCC_GetClockConfig() function to know which clock is
  *         currently used as system clock source.
  * @retval None
  */
HAL_StatusTypeDef HAL_RCC_ClockConfig(const RCC_ClkInitTypeDef  *pRCC_ClkInitStruct)
{
  uint32_t tickstart;

  /* Check Null pointer */
  if (pRCC_ClkInitStruct == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_RCC_CLOCKTYPE(pRCC_ClkInitStruct->ClockType));

  /* Increasing the BUS frequency divider ? */

  /*-------------------------- PCLK1 Configuration ---------------------------*/
  if (((pRCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1)
  {
    assert_param(IS_RCC_PCLK1(pRCC_ClkInitStruct->APB1CLKDivider));
    if ((pRCC_ClkInitStruct->APB1CLKDivider) > (RCC->CFGR2 & RCC_CFGR2_PPRE1))
    {
      MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE1, (pRCC_ClkInitStruct->APB1CLKDivider));
    }
  }

  /*-------------------------- PCLK2 Configuration ---------------------------*/
  if (((pRCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK2) == RCC_CLOCKTYPE_PCLK2)
  {
    assert_param(IS_RCC_PCLK2(pRCC_ClkInitStruct->APB2CLKDivider));
    if ((pRCC_ClkInitStruct->APB2CLKDivider) > (RCC->CFGR2 & RCC_CFGR2_PPRE2))
    {
      MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE2, (pRCC_ClkInitStruct->APB2CLKDivider));
    }
  }

  /*-------------------------- PCLK4 Configuration ---------------------------*/
  if (((pRCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK4) == RCC_CLOCKTYPE_PCLK4)
  {
    assert_param(IS_RCC_PCLK4(pRCC_ClkInitStruct->APB4CLKDivider));
    if ((pRCC_ClkInitStruct->APB4CLKDivider) > (RCC->CFGR2 & RCC_CFGR2_PPRE4))
    {
      MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE4, (pRCC_ClkInitStruct->APB4CLKDivider));
    }
  }

  /*-------------------------- PCLK5 Configuration ---------------------------*/
  if (((pRCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK5) == RCC_CLOCKTYPE_PCLK5)
  {
    assert_param(IS_RCC_PCLK5(pRCC_ClkInitStruct->APB5CLKDivider));
    if ((pRCC_ClkInitStruct->APB5CLKDivider) > (RCC->CFGR2 & RCC_CFGR2_PPRE5))
    {
      MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE5, (pRCC_ClkInitStruct->APB5CLKDivider));
    }
  }

  /*-------------------------- HCLK Configuration --------------------------*/
  if (((pRCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_HCLK) == RCC_CLOCKTYPE_HCLK)
  {
    assert_param(IS_RCC_HCLK(pRCC_ClkInitStruct->AHBCLKDivider));
    if ((pRCC_ClkInitStruct->AHBCLKDivider) > (RCC->CFGR2 & RCC_CFGR2_HPRE))
    {
      /* Set the new HCLK clock divider */
      MODIFY_REG(RCC->CFGR2, RCC_CFGR2_HPRE, pRCC_ClkInitStruct->AHBCLKDivider);
    }
  }

  /*------------------------- CPUCLK Configuration -------------------------*/
  if ((pRCC_ClkInitStruct->ClockType & RCC_CLOCKTYPE_CPUCLK) == RCC_CLOCKTYPE_CPUCLK)
  {
    assert_param(IS_RCC_CPUCLKSOURCE(pRCC_ClkInitStruct->CPUCLKSource));

    /* HSE is selected as CPU Clock Source */
    if (pRCC_ClkInitStruct->CPUCLKSource == RCC_CPUCLKSOURCE_HSE)
    {
      /* Check the HSE ready flag */
      if (LL_RCC_HSE_IsReady() == 0U)
      {
        return HAL_ERROR;
      }
    }
    /* PLL is selected as CPU Clock Source */
    else if (pRCC_ClkInitStruct->CPUCLKSource == RCC_CPUCLKSOURCE_IC1)
    {
      /* Check parameters */
      assert_param(IS_RCC_ICCLKSOURCE(pRCC_ClkInitStruct->IC1Selection.ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(pRCC_ClkInitStruct->IC1Selection.ClockDivider));

      /* ICx clock switch requires both origin and destination clock source to be active */
      /* Check IC1 origin and target clock sources availability */
      if (RCC_IC_CheckPLLSources(LL_RCC_IC1_GetSource(), pRCC_ClkInitStruct->IC1Selection.ClockSelection) != 1U)
      {
        return HAL_ERROR;
      }

      /* Configure IC1 source and divider */
      WRITE_REG(RCC->IC1CFGR, pRCC_ClkInitStruct->IC1Selection.ClockSelection | \
                ((pRCC_ClkInitStruct->IC1Selection.ClockDivider - 1U) << RCC_IC1CFGR_IC1INT_Pos));

      /* Enable IC1 */
      LL_RCC_IC1_Enable();
    }
    /* MSI is selected as CPU Clock Source */
    else if (pRCC_ClkInitStruct->CPUCLKSource == RCC_CPUCLKSOURCE_MSI)
    {
      /* Check the MSI ready flag */
      if (LL_RCC_MSI_IsReady() == 0U)
      {
        return HAL_ERROR;
      }
    }
    /* HSI is selected as CPU Clock Source */
    else
    {
      /* Check the HSI ready flag */
      if (LL_RCC_HSI_IsReady() == 0U)
      {
        return HAL_ERROR;
      }
    }

    /* Switch the CPU clock */
    MODIFY_REG(RCC->CFGR1, RCC_CFGR1_CPUSW, pRCC_ClkInitStruct->CPUCLKSource);

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    while (__HAL_RCC_GET_CPUCLK_SOURCE() != (pRCC_ClkInitStruct->CPUCLKSource << 4U))
    {
      if ((HAL_GetTick() - tickstart) > RCC_CLOCKSWITCH_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }

    /* Update the SystemCoreClock global variable with CPU clock */
    SystemCoreClock = HAL_RCC_GetCpuClockFreq();

  }

  /*------------------------- SYSCLK Configuration -------------------------*/
  if ((pRCC_ClkInitStruct->ClockType & RCC_CLOCKTYPE_SYSCLK) == RCC_CLOCKTYPE_SYSCLK)
  {
    assert_param(IS_RCC_SYSCLKSOURCE(pRCC_ClkInitStruct->SYSCLKSource));

    /* HSE is selected as System bus clock source */
    if (pRCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSE)
    {
      /* Check the HSE ready flag */
      if (LL_RCC_HSE_IsReady() == 0U)
      {
        return HAL_ERROR;
      }
    }
    /* PLL output is selected as System bus clock source */
    else if (pRCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_IC2_IC6_IC11)
    {
      /* Check parameters */
      assert_param(IS_RCC_ICCLKSOURCE(pRCC_ClkInitStruct->IC2Selection.ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(pRCC_ClkInitStruct->IC2Selection.ClockDivider));
      assert_param(IS_RCC_ICCLKSOURCE(pRCC_ClkInitStruct->IC6Selection.ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(pRCC_ClkInitStruct->IC6Selection.ClockDivider));
      assert_param(IS_RCC_ICCLKSOURCE(pRCC_ClkInitStruct->IC11Selection.ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(pRCC_ClkInitStruct->IC11Selection.ClockDivider));

      /* ICx clock switch requires both origin and destination clock source to be active */
      /* Check IC2/IC6/IC11 origin and target clock sources availability */
      if (RCC_IC_CheckPLLSources(LL_RCC_IC2_GetSource(), pRCC_ClkInitStruct->IC2Selection.ClockSelection) != 1U)
      {
        return HAL_ERROR;
      }
      if (RCC_IC_CheckPLLSources(LL_RCC_IC6_GetSource(), pRCC_ClkInitStruct->IC6Selection.ClockSelection) != 1U)
      {
        return HAL_ERROR;
      }
      if (RCC_IC_CheckPLLSources(LL_RCC_IC11_GetSource(), pRCC_ClkInitStruct->IC11Selection.ClockSelection) != 1U)
      {
        return HAL_ERROR;
      }

      /* Configure IC2, IC6 and IC11 sources and dividers */
      WRITE_REG(RCC->IC2CFGR, pRCC_ClkInitStruct->IC2Selection.ClockSelection | \
                ((pRCC_ClkInitStruct->IC2Selection.ClockDivider - 1U) << RCC_IC2CFGR_IC2INT_Pos));
      WRITE_REG(RCC->IC6CFGR, pRCC_ClkInitStruct->IC6Selection.ClockSelection | \
                ((pRCC_ClkInitStruct->IC6Selection.ClockDivider - 1U) << RCC_IC6CFGR_IC6INT_Pos));
      WRITE_REG(RCC->IC11CFGR, pRCC_ClkInitStruct->IC11Selection.ClockSelection | \
                ((pRCC_ClkInitStruct->IC11Selection.ClockDivider - 1U) << RCC_IC11CFGR_IC11INT_Pos));

      /* Require to have IC2, IC6 and IC11 outputs enabled */
      WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC2ENS | RCC_DIVENSR_IC6ENS | RCC_DIVENSR_IC11ENS);
    }
    /* HSI is selected as System bus clock source */
    else if (pRCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSI)
    {
      /* Check the HSI ready flag */
      if (LL_RCC_HSI_IsReady() == 0U)
      {
        return HAL_ERROR;
      }
    }
    /* MSI is selected as System bus clock source */
    else
    {
      /* Check the MSI ready flag */
      if (LL_RCC_MSI_IsReady() == 0U)
      {
        return HAL_ERROR;
      }
    }

    /* Switch the system bus clocks */
    MODIFY_REG(RCC->CFGR1, RCC_CFGR1_SYSSW, pRCC_ClkInitStruct->SYSCLKSource);

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    while (__HAL_RCC_GET_SYSCLK_SOURCE() != (pRCC_ClkInitStruct->SYSCLKSource << 4U))
    {
      if ((HAL_GetTick() - tickstart) > RCC_CLOCKSWITCH_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }
  }

  /* Decreasing the BUS frequency divider ? */

  /*-------------------------- HCLK Configuration --------------------------*/
  if (((pRCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_HCLK) == RCC_CLOCKTYPE_HCLK)
  {
    assert_param(IS_RCC_HCLK(pRCC_ClkInitStruct->AHBCLKDivider));
    if ((pRCC_ClkInitStruct->AHBCLKDivider) < (RCC->CFGR2 & RCC_CFGR2_HPRE))
    {
      /* Set the new HCLK clock divider */
      MODIFY_REG(RCC->CFGR2, RCC_CFGR2_HPRE, pRCC_ClkInitStruct->AHBCLKDivider);
    }
  }

  /*-------------------------- PCLK1 Configuration ---------------------------*/
  if (((pRCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1)
  {
    assert_param(IS_RCC_PCLK1(pRCC_ClkInitStruct->APB1CLKDivider));
    if ((pRCC_ClkInitStruct->APB1CLKDivider) < (RCC->CFGR2 & RCC_CFGR2_PPRE1))
    {
      MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE1, (pRCC_ClkInitStruct->APB1CLKDivider));
    }
  }

  /*-------------------------- PCLK2 Configuration ---------------------------*/
  if (((pRCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK2) == RCC_CLOCKTYPE_PCLK2)
  {
    assert_param(IS_RCC_PCLK2(pRCC_ClkInitStruct->APB2CLKDivider));
    if ((pRCC_ClkInitStruct->APB2CLKDivider) < (RCC->CFGR2 & RCC_CFGR2_PPRE2))
    {
      MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE2, (pRCC_ClkInitStruct->APB2CLKDivider));
    }
  }

  /*-------------------------- PCLK4 Configuration ---------------------------*/
  if (((pRCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK4) == RCC_CLOCKTYPE_PCLK4)
  {
    assert_param(IS_RCC_PCLK4(pRCC_ClkInitStruct->APB4CLKDivider));
    if ((pRCC_ClkInitStruct->APB4CLKDivider) < (RCC->CFGR2 & RCC_CFGR2_PPRE4))
    {
      MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE4, (pRCC_ClkInitStruct->APB4CLKDivider));
    }
  }

  /*-------------------------- PCLK5 Configuration ---------------------------*/
  if (((pRCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK5) == RCC_CLOCKTYPE_PCLK5)
  {
    assert_param(IS_RCC_PCLK5(pRCC_ClkInitStruct->APB5CLKDivider));
    if ((pRCC_ClkInitStruct->APB5CLKDivider) < (RCC->CFGR2 & RCC_CFGR2_PPRE5))
    {
      MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE5, (pRCC_ClkInitStruct->APB5CLKDivider));
    }
  }

  /* Configure the source of time base considering new system clocks settings*/
  return HAL_InitTick(uwTickPrio);
}

/**
  * @}
  */

/** @defgroup RCC_Group2 Peripheral Control functions
  *  @brief   RCC clocks control functions
  *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the RCC Clocks
    frequencies.

@endverbatim
  * @{
  */

/**
  * @brief  Selects the clock source to output on MCO1 pin(PA8) or on MCO2 pin(PC9).
  * @note   PWR VDDIO4 Input Independent I/O supply 4 shall be previously set up for PC9
  *         with HAL_PWREx_EnableVddIO4()
  * @note   The MCO switch to the new clock source only occurs when the previous clock source is active
  *         (dynamic switch).
  * @param  RCC_MCOx specifies the output direction for the clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCO1 Clock source to output on MCO1 pin(PA8).
  *            @arg RCC_MCO2 Clock source to output on MCO2 pin(PC9).
  * @param  RCC_MCOSource specifies the clock source to output.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCO1SOURCE_LSI LSI clock selected as MCO1 source
  *            @arg RCC_MCO1SOURCE_LSE LSE clock selected as MCO1 source
  *            @arg RCC_MCO1SOURCE_MSI MSI clock selected as MCO1 source
  *            @arg RCC_MCO1SOURCE_HSI HSI clock selected as MCO1 source
  *            @arg RCC_MCO1SOURCE_HSE HSE clock selected as MCO1 source
  *            @arg RCC_MCO1SOURCE_IC5 IC5 clock selected as MCO1 source
  *            @arg RCC_MCO1SOURCE_IC10 IC10 clock selected as MCO1 source
  *            @arg RCC_MCO1SOURCE_SYSA CPU clock (SYSA) selected as MCO1 source
  *            @arg RCC_MCO2SOURCE_LSI LSI clock selected as MCO2 source
  *            @arg RCC_MCO2SOURCE_LSE LSE clock selected as MCO2 source
  *            @arg RCC_MCO2SOURCE_MSI MSI clock selected as MCO2 source
  *            @arg RCC_MCO2SOURCE_HSI HSI clock selected as MCO2 source
  *            @arg RCC_MCO2SOURCE_HSE HSE clock selected as MCO2 source
  *            @arg RCC_MCO2SOURCE_IC15 IC15 clock selected as MCO2 source
  *            @arg RCC_MCO2SOURCE_IC20 IC20 clock selected as MCO2 source
  *            @arg RCC_MCO2SOURCE_SYSB Bus clock (SYSB) selected as MCO2 source
  * @param  RCC_MCODiv specifies the MCOx pre-scaler.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCODIV_1 up to RCC_MCODIV_16  : divider applied to MCOx clock
  * @retval None
  */
void HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Check the parameters */
  assert_param(IS_RCC_MCO(RCC_MCOx));
  assert_param(IS_RCC_MCODIV(RCC_MCODiv));

  /* RCC_MCO1 */
  if (RCC_MCOx == RCC_MCO1)
  {
    assert_param(IS_RCC_MCO1SOURCE(RCC_MCOSource));

    /* MCO1 Clock Enable */
    RCC_MCO1_CLK_ENABLE();

    /* Configure the MCO1 pin in alternate function mode */
    GPIO_InitStruct.Pin = RCC_MCO1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
    HAL_GPIO_Init(RCC_MCO1_GPIO_PORT, &GPIO_InitStruct);

    /* Configure MCO1 */
    LL_RCC_ConfigMCO(RCC_MCOSource, RCC_MCODiv);

    /* Enable IC5 if requested as source to MCO1 */
    if (RCC_MCOSource == RCC_MCO1SOURCE_IC5)
    {
      LL_RCC_IC5_Enable();
    }
    /* Enable IC10 if requested as source to MCO1 */
    else if (RCC_MCOSource == RCC_MCO1SOURCE_IC10)
    {
      LL_RCC_IC10_Enable();
    }
    else
    {
      /* Nothing to do as not ICx source */
    }
    /* Enable MC01 output */
    LL_RCC_EnableMCO(LL_RCC_MCO1);
  }
  else
  {
    assert_param(IS_RCC_MCO2SOURCE(RCC_MCOSource));

    /* MCO2 Clock Enable */
    RCC_MCO2_CLK_ENABLE();

    /* Configure the MCO2 pin in alternate function mode */
    GPIO_InitStruct.Pin = RCC_MCO2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
    HAL_GPIO_Init(RCC_MCO2_GPIO_PORT, &GPIO_InitStruct);

    /* Configure MCO2 */
    LL_RCC_ConfigMCO(RCC_MCOSource, ((RCC_CCIPR5_MCO2PRE << 16U) | ((RCC_MCODiv & RCC_CCIPR5_MCO1PRE) << 8U)));

    /* Enable IC15 if requested as source to MCO2 */
    if (RCC_MCOSource == RCC_MCO2SOURCE_IC15)
    {
      LL_RCC_IC15_Enable();
    }
    /* Enable IC20 if requested as source to MCO2 */
    else if (RCC_MCOSource == RCC_MCO2SOURCE_IC20)
    {
      LL_RCC_IC20_Enable();
    }
    else
    {
      /* Nothing to do as not ICx source */
    }
    /* Enable MC02 output */
    LL_RCC_EnableMCO(LL_RCC_MCO2);
  }
}

/**
  * @brief  Enable the HSE Clock Security System.
  * @note   If a failure is detected on the HSE oscillator clock, this oscillator
  *         is automatically disabled and an interrupt is generated to inform the
  *         software about the failure (Clock Security System Interrupt, CSSI),
  *         allowing the MCU to perform rescue operations. The CSSI is linked to
  *         the Cortex-M NMI (Non-Mask-able Interrupt) exception vector.
  * @note   The HSE Clock Security System may be enabled even if the HSE is not enabled
  *         but will be activated when the HSE is enabled and ready.
  * @note   It is not possible to disable the HSE Clock Security System.
  * @retval None
  */
void HAL_RCC_EnableCSS(void)
{
  LL_RCC_HSE_EnableCSS();
}

/**
  * @brief  Returns the CPU clock (sysa_ck) frequency
  *
  * @note   The CPU clock frequency computed by this function may be not the real
  *         frequency in the chip. It is calculated based on the predefined
  *         constant and the selected clock source:
  * @note     If CPUCLK source is HSI, function returns values based on HSI_VALUE(**)
  * @note     If CPUCLK source is MSI, function returns values based on MSI_VALUE(*)
  * @note     If CPUCLK source is HSE, function returns values based on HSE_VALUE(***)
  * @note     If CPUCLK source is IC1, function returns values based on HSI_VALUE(*),
  *           MSI_VALUE(**) or HSE_VALUE(***) multiplied/divided by the PLL factors.
  * @note     (*) HSI_VALUE is a constant defined in stm32n6xx_hal_conf.h file (default value
  *               64 MHz) but the real value may vary depending on the variations
  *               in voltage and temperature.
  * @note     (**) MSI_VALUE is a constant defined in stm32n6xx_hal_conf.h file (default value
  *               4 MHz) but the real value may vary depending on the variations
  *               in voltage and temperature.
  * @note     (***) HSE_VALUE is a constant defined in stm32n6xx_hal_conf.h file (default value
  *                25 MHz), user has to ensure that HSE_VALUE is same as the real
  *                frequency of the crystal used. Otherwise, this function may
  *                have wrong result.
  *
  * @note   The result of this function could be not correct when using fractional
  *         value for HSE crystal.
  *
  * @note   This function can be used by the user application to compute the
  *         baud rate for the communication peripherals or configure other parameters.
  *
  * @note   Each time CPUCLK changes, this function must be called by the user application
  *         to update the CPUCLK value. Otherwise, any configuration based on this function
  *         will be incorrect.
  *
  * @retval CPUCLK frequency
  */
uint32_t HAL_RCC_GetCpuClockFreq(void)
{
  uint32_t frequency = 0U;
  uint32_t ic_divider;

  /* Get CPUCLK source -------------------------------------------------------*/
  switch (LL_RCC_GetCpuClkSource())
  {
    /* No check on Ready: Won't be selected by hardware if not */
    case LL_RCC_CPU_CLKSOURCE_STATUS_HSI:
      frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      break;

    case LL_RCC_CPU_CLKSOURCE_STATUS_MSI:
      if (LL_RCC_MSI_GetFrequency() == LL_RCC_MSI_FREQ_4MHZ)
      {
        frequency = MSI_VALUE;
      }
      else
      {
        frequency = 16000000UL;
      }
      break;

    case LL_RCC_CPU_CLKSOURCE_STATUS_HSE:
      frequency = HSE_VALUE;
      break;

    case LL_RCC_CPU_CLKSOURCE_STATUS_IC1:
      ic_divider = LL_RCC_IC1_GetDivider();
      switch (LL_RCC_IC1_GetSource())
      {
        case LL_RCC_ICCLKSOURCE_PLL1:
          frequency = HAL_RCCEx_GetPLL1CLKFreq();
          frequency = frequency / ic_divider;
          break;
        case LL_RCC_ICCLKSOURCE_PLL2:
          frequency = HAL_RCCEx_GetPLL2CLKFreq();
          frequency = frequency / ic_divider;
          break;
        case LL_RCC_ICCLKSOURCE_PLL3:
          frequency = HAL_RCCEx_GetPLL3CLKFreq();
          frequency = frequency / ic_divider;
          break;
        case LL_RCC_ICCLKSOURCE_PLL4:
          frequency = HAL_RCCEx_GetPLL4CLKFreq();
          frequency = frequency / ic_divider;
          break;
        default:
          /* Unexpected case */
          break;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Returns the SYSCLK bus (sysb_ck) frequency
  *
  * @note   The system bus frequency computed by this function may be not the real
  *         frequency in the chip. It is calculated based on the predefined
  *         constant and the selected clock source:
  * @note     If SYSCLK source is HSI, function returns values based on HSI_VALUE(**)
  * @note     If SYSCLK source is MSI, function returns values based on MSI_VALUE(*)
  * @note     If SYSCLK source is HSE, function returns values based on HSE_VALUE(***)
  * @note     If SYSCLK source is IC2, function returns values based on HSI_VALUE(*),
  *           MSI_VALUE(**) or HSE_VALUE(***) multiplied/divided by the PLL factors.
  * @note     (*) HSI_VALUE is a constant defined in stm32n6xx_hal_conf.h file (default value
  *               64 MHz) but the real value may vary depending on the variations
  *               in voltage and temperature.
  * @note     (**) MSI_VALUE is a constant defined in stm32n6xx_hal_conf.h file (default value
  *               4 MHz) but the real value may vary depending on the variations
  *               in voltage and temperature.
  * @note     (***) HSE_VALUE is a constant defined in stm32n6xx_hal_conf.h file (default value
  *                25 MHz), user has to ensure that HSE_VALUE is same as the real
  *                frequency of the crystal used. Otherwise, this function may
  *                have wrong result.
  *
  * @note   The result of this function could be not correct when using fractional
  *         value for HSE crystal.
  *
  * @note   This function can be used by the user application to compute the
  *         baud rate for the communication peripherals or configure other parameters.
  *
  * @note   Each time SYSCLK changes, this function must be called by the user application
  *         to update the SYSCLK value. Otherwise, any configuration based on this function
  *         will be incorrect.
  *
  * @retval SYSCLK frequency
  */
uint32_t HAL_RCC_GetSysClockFreq(void)
{
  uint32_t frequency = 0U;
  uint32_t ic_divider;

  /* Get SYSCLK source -------------------------------------------------------*/
  switch (LL_RCC_GetSysClkSource())
  {
    /* No check on Ready: Won't be selected by hardware if not */
    case LL_RCC_SYS_CLKSOURCE_STATUS_HSI:
      frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      break;

    case LL_RCC_SYS_CLKSOURCE_STATUS_MSI:
      if (LL_RCC_MSI_GetFrequency() == LL_RCC_MSI_FREQ_4MHZ)
      {
        frequency = MSI_VALUE;
      }
      else
      {
        frequency = 16000000UL;
      }
      break;

    case LL_RCC_SYS_CLKSOURCE_STATUS_HSE:
      frequency = HSE_VALUE;
      break;

    case LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11:
      ic_divider = LL_RCC_IC2_GetDivider();
      switch (LL_RCC_IC2_GetSource())
      {
        case LL_RCC_ICCLKSOURCE_PLL1:
          frequency = HAL_RCCEx_GetPLL1CLKFreq();
          frequency = frequency / ic_divider;
          break;
        case LL_RCC_ICCLKSOURCE_PLL2:
          frequency = HAL_RCCEx_GetPLL2CLKFreq();
          frequency = frequency / ic_divider;
          break;
        case LL_RCC_ICCLKSOURCE_PLL3:
          frequency = HAL_RCCEx_GetPLL3CLKFreq();
          frequency = frequency / ic_divider;
          break;
        case LL_RCC_ICCLKSOURCE_PLL4:
          frequency = HAL_RCCEx_GetPLL4CLKFreq();
          frequency = frequency / ic_divider;
          break;
        default:
          /* Unexpected case */
          break;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return the HCLK frequency.
  * @retval HCLK frequency in Hz
  */
uint32_t HAL_RCC_GetHCLKFreq(void)
{
  return LL_RCC_CALC_HCLK_FREQ(HAL_RCC_GetSysClockFreq(), LL_RCC_GetAHBPrescaler());
}

/**
  * @brief  Return the PCLK1 frequency.
  * @note   Each time PCLK1 changes, this function must be called to update the
  *         right PCLK1 value. Otherwise, any configuration based on this function will be incorrect.
  * @retval PCLK1 frequency in Hz
  */
uint32_t HAL_RCC_GetPCLK1Freq(void)
{
  return LL_RCC_CALC_PCLK1_FREQ(LL_RCC_CALC_HCLK_FREQ(HAL_RCC_GetSysClockFreq(), LL_RCC_GetAHBPrescaler()),
                                LL_RCC_GetAPB1Prescaler());
}

/**
  * @brief  Return the PCLK2 frequency.
  * @note   Each time PCLK2 changes, this function must be called to update the
  *         right PCLK2 value. Otherwise, any configuration based on this function will be incorrect.
  * @retval PCLK2 frequency in Hz
  */
uint32_t HAL_RCC_GetPCLK2Freq(void)
{
  return LL_RCC_CALC_PCLK2_FREQ(LL_RCC_CALC_HCLK_FREQ(HAL_RCC_GetSysClockFreq(), LL_RCC_GetAHBPrescaler()),
                                LL_RCC_GetAPB2Prescaler());
}

/**
  * @brief  Return the PCLK4 frequency.
  * @note   Each time PCLK4 changes, this function must be called to update the
  *         right PCLK4 value. Otherwise, any configuration based on this function will be incorrect.
  * @retval PCLK4 frequency in Hz
  */
uint32_t HAL_RCC_GetPCLK4Freq(void)
{
  return LL_RCC_CALC_PCLK4_FREQ(LL_RCC_CALC_HCLK_FREQ(HAL_RCC_GetSysClockFreq(), LL_RCC_GetAHBPrescaler()),
                                LL_RCC_GetAPB4Prescaler());
}

/**
  * @brief  Return the PCLK5 frequency.
  * @note   Each time PCLK5 changes, this function must be called to update the
  *         right PCLK5 value. Otherwise, any configuration based on this function will be incorrect.
  * @retval PCLK5 frequency in Hz
  */
uint32_t HAL_RCC_GetPCLK5Freq(void)
{
  return LL_RCC_CALC_PCLK5_FREQ(LL_RCC_CALC_HCLK_FREQ(HAL_RCC_GetSysClockFreq(), LL_RCC_GetAHBPrescaler()),
                                LL_RCC_GetAPB5Prescaler());
}

/**
  * @brief  Get the oscillators status configuration in RCC_OscInitStruct according to
  *         the internal RCC configuration registers.
  * @param  pRCC_OscInitStruct  Pointer to an RCC_OscInitTypeDef structure that
  *         will return the configuration.
  * @retval None
  */
void HAL_RCC_GetOscConfig(RCC_OscInitTypeDef  *pRCC_OscInitStruct)
{
  uint32_t sr_value;
  uint32_t cfgr_value;

  /* Get the status register value */
  sr_value = RCC->SR;

  /* Set all possible values for the Oscillator type parameters --------------*/
  pRCC_OscInitStruct->OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_MSI | \
                                       RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI;

  /* Get the HSE configuration -----------------------------------------------*/
  if ((sr_value & RCC_SR_HSERDY) != 0UL)
  {
    cfgr_value = RCC->HSECFGR;
    if ((cfgr_value & (RCC_HSECFGR_HSEBYP | RCC_HSECFGR_HSEEXT)) == RCC_HSECFGR_HSEBYP)
    {
      pRCC_OscInitStruct->HSEState = RCC_HSE_BYPASS;
    }
    else if ((cfgr_value & (RCC_HSECFGR_HSEBYP | RCC_HSECFGR_HSEEXT)) == (RCC_HSECFGR_HSEBYP | RCC_HSECFGR_HSEEXT))
    {
      pRCC_OscInitStruct->HSEState = RCC_HSE_BYPASS_DIGITAL;
    }
    else
    {
      pRCC_OscInitStruct->HSEState = RCC_HSE_ON;
    }
  }
  else
  {
    pRCC_OscInitStruct->HSEState = RCC_HSE_OFF;
  }

  /* Get the MSI configuration -----------------------------------------------*/
  if ((sr_value & RCC_SR_MSIRDY) != 0UL)
  {
    pRCC_OscInitStruct->MSIState = RCC_MSI_ON;
  }
  else
  {
    pRCC_OscInitStruct->MSIState = RCC_MSI_OFF;
  }

  cfgr_value = RCC->MSICFGR;
  pRCC_OscInitStruct->MSIFrequency = (cfgr_value & RCC_MSICFGR_MSIFREQSEL);
  pRCC_OscInitStruct->MSICalibrationValue = ((cfgr_value & RCC_MSICFGR_MSITRIM) >> RCC_MSICFGR_MSITRIM_Pos);

  /* Get the HSI configuration -----------------------------------------------*/
  if ((sr_value & RCC_SR_HSIRDY) != 0UL)
  {
    pRCC_OscInitStruct->HSIState = RCC_HSI_ON;
  }
  else
  {
    pRCC_OscInitStruct->HSIState = RCC_HSI_OFF;
  }

  cfgr_value = RCC->HSICFGR;
  pRCC_OscInitStruct->HSIDiv = (cfgr_value & RCC_HSICFGR_HSIDIV);
  pRCC_OscInitStruct->HSICalibrationValue = ((cfgr_value & RCC_HSICFGR_HSITRIM) >> RCC_HSICFGR_HSITRIM_Pos);

  /* Get the LSE configuration -----------------------------------------------*/
  if ((sr_value & RCC_SR_LSERDY) != 0UL)
  {
    cfgr_value = RCC->LSECFGR;
    if ((cfgr_value & (RCC_LSECFGR_LSEBYP | RCC_LSECFGR_LSEEXT)) == RCC_LSECFGR_LSEBYP)
    {
      pRCC_OscInitStruct->LSEState = RCC_LSE_BYPASS;
    }
    else if ((cfgr_value & (RCC_LSECFGR_LSEBYP | RCC_LSECFGR_LSEEXT)) == (RCC_LSECFGR_LSEBYP | RCC_LSECFGR_LSEEXT))
    {
      pRCC_OscInitStruct->LSEState = RCC_LSE_BYPASS_DIGITAL;
    }
    else
    {
      pRCC_OscInitStruct->LSEState = RCC_LSE_ON;
    }
  }
  else
  {
    pRCC_OscInitStruct->LSEState = RCC_LSE_OFF;
  }

  /* Get the LSI configuration -----------------------------------------------*/
  if ((sr_value & RCC_SR_LSIRDY) != 0UL)
  {
    pRCC_OscInitStruct->LSIState = RCC_LSI_ON;
  }
  else
  {
    pRCC_OscInitStruct->LSIState = RCC_LSI_OFF;
  }

  /* Get the PLL1 configuration -----------------------------------------------*/
  if ((sr_value & RCC_SR_PLL1RDY) != 0UL)
  {
    cfgr_value = RCC->PLL1CFGR1;
    pRCC_OscInitStruct->PLL1.PLLState = RCC_PLL_ON;
    pRCC_OscInitStruct->PLL1.PLLSource = (cfgr_value & RCC_PLL1CFGR1_PLL1SEL);
    pRCC_OscInitStruct->PLL1.PLLM = ((cfgr_value & RCC_PLL1CFGR1_PLL1DIVM) >> RCC_PLL1CFGR1_PLL1DIVM_Pos);
    pRCC_OscInitStruct->PLL1.PLLN = ((cfgr_value & RCC_PLL1CFGR1_PLL1DIVN) >> RCC_PLL1CFGR1_PLL1DIVN_Pos);
    pRCC_OscInitStruct->PLL1.PLLFractional = (READ_BIT(RCC->PLL1CFGR2, RCC_PLL1CFGR2_PLL1DIVNFRAC) >> \
                                              RCC_PLL1CFGR2_PLL1DIVNFRAC_Pos);
    cfgr_value = RCC->PLL1CFGR3;
    pRCC_OscInitStruct->PLL1.PLLP1 = ((cfgr_value & RCC_PLL1CFGR3_PLL1PDIV1) >> RCC_PLL1CFGR3_PLL1PDIV1_Pos);
    pRCC_OscInitStruct->PLL1.PLLP2 = ((cfgr_value & RCC_PLL1CFGR3_PLL1PDIV2) >> RCC_PLL1CFGR3_PLL1PDIV2_Pos);
  }
  else
  {
    cfgr_value = RCC->PLL1CFGR1;
    if ((cfgr_value & RCC_PLL1CFGR1_PLL1BYP) != 0UL)
    {
      pRCC_OscInitStruct->PLL1.PLLState = RCC_PLL_BYPASS;
    }
    else
    {
      pRCC_OscInitStruct->PLL1.PLLState = RCC_PLL_OFF;
    }
  }

  /* Get the PLL2 configuration -----------------------------------------------*/
  if ((sr_value & RCC_SR_PLL2RDY) != 0UL)
  {
    cfgr_value = RCC->PLL2CFGR1;
    pRCC_OscInitStruct->PLL2.PLLState = RCC_PLL_ON;
    pRCC_OscInitStruct->PLL2.PLLSource = (cfgr_value & RCC_PLL2CFGR1_PLL2SEL);
    pRCC_OscInitStruct->PLL2.PLLM = ((cfgr_value & RCC_PLL2CFGR1_PLL2DIVM) >> RCC_PLL2CFGR1_PLL2DIVM_Pos);
    pRCC_OscInitStruct->PLL2.PLLN = ((cfgr_value & RCC_PLL2CFGR1_PLL2DIVN) >> RCC_PLL2CFGR1_PLL2DIVN_Pos);
    pRCC_OscInitStruct->PLL2.PLLFractional = (READ_BIT(RCC->PLL2CFGR2, RCC_PLL2CFGR2_PLL2DIVNFRAC) >> \
                                              RCC_PLL2CFGR2_PLL2DIVNFRAC_Pos);
    cfgr_value = RCC->PLL2CFGR3;
    pRCC_OscInitStruct->PLL2.PLLP1 = ((cfgr_value & RCC_PLL2CFGR3_PLL2PDIV1) >> RCC_PLL2CFGR3_PLL2PDIV1_Pos);
    pRCC_OscInitStruct->PLL2.PLLP2 = ((cfgr_value & RCC_PLL2CFGR3_PLL2PDIV2) >> RCC_PLL2CFGR3_PLL2PDIV2_Pos);
  }
  else
  {
    cfgr_value = RCC->PLL2CFGR1;
    if ((cfgr_value & RCC_PLL2CFGR1_PLL2BYP) != 0UL)
    {
      pRCC_OscInitStruct->PLL2.PLLState = RCC_PLL_BYPASS;
    }
    else
    {
      pRCC_OscInitStruct->PLL2.PLLState = RCC_PLL_OFF;
    }
  }

  /* Get the PLL3 configuration -----------------------------------------------*/
  if ((sr_value & RCC_SR_PLL3RDY) != 0UL)
  {
    cfgr_value = RCC->PLL3CFGR1;
    pRCC_OscInitStruct->PLL3.PLLState = RCC_PLL_ON;
    pRCC_OscInitStruct->PLL3.PLLSource = (cfgr_value & RCC_PLL3CFGR1_PLL3SEL);
    pRCC_OscInitStruct->PLL3.PLLM = ((cfgr_value & RCC_PLL3CFGR1_PLL3DIVM) >> RCC_PLL3CFGR1_PLL3DIVM_Pos);
    pRCC_OscInitStruct->PLL3.PLLN = ((cfgr_value & RCC_PLL3CFGR1_PLL3DIVN) >> RCC_PLL3CFGR1_PLL3DIVN_Pos);
    pRCC_OscInitStruct->PLL3.PLLFractional = (READ_BIT(RCC->PLL3CFGR2, RCC_PLL3CFGR2_PLL3DIVNFRAC) >> \
                                              RCC_PLL3CFGR2_PLL3DIVNFRAC_Pos);
    cfgr_value = RCC->PLL3CFGR3;
    pRCC_OscInitStruct->PLL3.PLLP1 = ((cfgr_value & RCC_PLL3CFGR3_PLL3PDIV1) >> RCC_PLL3CFGR3_PLL3PDIV1_Pos);
    pRCC_OscInitStruct->PLL3.PLLP2 = ((cfgr_value & RCC_PLL3CFGR3_PLL3PDIV2) >> RCC_PLL3CFGR3_PLL3PDIV2_Pos);
  }
  else
  {
    cfgr_value = RCC->PLL3CFGR1;
    if ((cfgr_value & RCC_PLL3CFGR1_PLL3BYP) != 0UL)
    {
      pRCC_OscInitStruct->PLL3.PLLState = RCC_PLL_BYPASS;
    }
    else
    {
      pRCC_OscInitStruct->PLL3.PLLState = RCC_PLL_OFF;
    }
  }

  /* Get the PLL4 configuration -----------------------------------------------*/
  if ((sr_value & RCC_SR_PLL4RDY) != 0UL)
  {
    cfgr_value = RCC->PLL4CFGR1;
    pRCC_OscInitStruct->PLL4.PLLState = RCC_PLL_ON;
    pRCC_OscInitStruct->PLL4.PLLSource = (cfgr_value & RCC_PLL4CFGR1_PLL4SEL);
    pRCC_OscInitStruct->PLL4.PLLM = ((cfgr_value & RCC_PLL4CFGR1_PLL4DIVM) >> RCC_PLL4CFGR1_PLL4DIVM_Pos);
    pRCC_OscInitStruct->PLL4.PLLN = ((cfgr_value & RCC_PLL4CFGR1_PLL4DIVN) >> RCC_PLL4CFGR1_PLL4DIVN_Pos);
    pRCC_OscInitStruct->PLL4.PLLFractional = (READ_BIT(RCC->PLL4CFGR2, RCC_PLL4CFGR2_PLL4DIVNFRAC) >> \
                                              RCC_PLL4CFGR2_PLL4DIVNFRAC_Pos);
    cfgr_value = RCC->PLL4CFGR3;
    pRCC_OscInitStruct->PLL4.PLLP1 = ((cfgr_value & RCC_PLL4CFGR3_PLL4PDIV1) >> RCC_PLL4CFGR3_PLL4PDIV1_Pos);
    pRCC_OscInitStruct->PLL4.PLLP2 = ((cfgr_value & RCC_PLL4CFGR3_PLL4PDIV2) >> RCC_PLL4CFGR3_PLL4PDIV2_Pos);
  }
  else
  {
    cfgr_value = RCC->PLL4CFGR1;
    if ((cfgr_value & RCC_PLL4CFGR1_PLL4BYP) != 0UL)
    {
      pRCC_OscInitStruct->PLL4.PLLState = RCC_PLL_BYPASS;
    }
    else
    {
      pRCC_OscInitStruct->PLL4.PLLState = RCC_PLL_OFF;
    }
  }
}

/**
  * @brief  Get the clocks status configuration in RCC_ClkInitStruct according to
  *         the internal RCC configuration registers.
  * @param  pRCC_ClkInitStruct  Pointer to an RCC_ClkInitTypeDef structure that
  *         will return the configuration.
  * @retval None
  */
void HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef  *pRCC_ClkInitStruct)
{
  uint32_t cfgr_value;

  /* Set all possible values for the Clock type parameter --------------------*/
  pRCC_ClkInitStruct->ClockType = RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK | \
                                  RCC_CLOCKTYPE_HCLK   | \
                                  RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2 | \
                                  RCC_CLOCKTYPE_PCLK4  | RCC_CLOCKTYPE_PCLK5;

  /* Get the configuration register 1 value */
  cfgr_value = RCC->CFGR1;

  /* Get the active CPU source -----------------------------------------------*/
  pRCC_ClkInitStruct->CPUCLKSource = (cfgr_value & RCC_CFGR1_CPUSWS) >> 4U;

  /* Get the active SYSCLK bus source ----------------------------------------*/
  pRCC_ClkInitStruct->SYSCLKSource = (cfgr_value & RCC_CFGR1_SYSSWS) >> 4U;

  /* Get the configuration register 2 value */
  cfgr_value = RCC->CFGR2;

  /* Get the HCLK configuration ----------------------------------------------*/
  pRCC_ClkInitStruct->AHBCLKDivider = (cfgr_value & RCC_CFGR2_HPRE);

  /* Get the APB1 configuration ----------------------------------------------*/
  pRCC_ClkInitStruct->APB1CLKDivider = (cfgr_value & RCC_CFGR2_PPRE1);

  /* Get the APB2 configuration ----------------------------------------------*/
  pRCC_ClkInitStruct->APB2CLKDivider = (cfgr_value & RCC_CFGR2_PPRE2);

  /* Get the APB4 configuration ----------------------------------------------*/
  pRCC_ClkInitStruct->APB4CLKDivider = (cfgr_value & RCC_CFGR2_PPRE4);

  /* Get the APB5 configuration ----------------------------------------------*/
  pRCC_ClkInitStruct->APB5CLKDivider = (cfgr_value & RCC_CFGR2_PPRE5);

  /* Get the IC1 configuration -----------------------------------------------*/
  cfgr_value = RCC->IC1CFGR;
  pRCC_ClkInitStruct->IC1Selection.ClockSelection = cfgr_value & RCC_IC1CFGR_IC1SEL;
  pRCC_ClkInitStruct->IC1Selection.ClockDivider = ((cfgr_value & RCC_IC1CFGR_IC1INT) >> RCC_IC1CFGR_IC1INT_Pos) + 1U;

  /* Get the IC2 configuration -----------------------------------------------*/
  cfgr_value = RCC->IC2CFGR;
  pRCC_ClkInitStruct->IC2Selection.ClockSelection = cfgr_value & RCC_IC2CFGR_IC2SEL;
  pRCC_ClkInitStruct->IC2Selection.ClockDivider = ((cfgr_value & RCC_IC2CFGR_IC2INT) >> RCC_IC2CFGR_IC2INT_Pos) + 1U;

  /* Get the IC6 configuration -----------------------------------------------*/
  cfgr_value = RCC->IC6CFGR;
  pRCC_ClkInitStruct->IC6Selection.ClockSelection = cfgr_value & RCC_IC6CFGR_IC6SEL;
  pRCC_ClkInitStruct->IC6Selection.ClockDivider = ((cfgr_value & RCC_IC6CFGR_IC6INT) >> RCC_IC6CFGR_IC6INT_Pos) + 1U;

  /* Get the IC11 configuration ----------------------------------------------*/
  cfgr_value = RCC->IC11CFGR;
  pRCC_ClkInitStruct->IC11Selection.ClockSelection = cfgr_value & RCC_IC11CFGR_IC11SEL;
  pRCC_ClkInitStruct->IC11Selection.ClockDivider = ((cfgr_value & RCC_IC11CFGR_IC11INT) >> RCC_IC11CFGR_IC11INT_Pos) + 1U;
}

/**
  * @brief This function handles the RCC HSE CSS interrupt request.
  * @note This API should be called under the NMI_Handler().
  * @retval None
  */
void HAL_RCC_NMI_IRQHandler(void)
{
  /* Check RCC HCECSSF interrupt flag  */
  if (LL_RCC_IsActiveFlag_HSECSS() == 1U)
  {
    /* Clear RCC HSE Clock Security System pending flag */
    LL_RCC_ClearFlag_HSECSS();

    /* RCC HSE Clock Security System interrupt user callback */
    HAL_RCC_CSSCallback();
  }
}

/**
  * @brief  RCC HSE Clock Security System interrupt callback
  * @retval none
  */
__weak void HAL_RCC_CSSCallback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RCC_CSSCallback could be implemented in the user file
   */
}

/**
  * @}
  */


/** @defgroup RCC_Exported_Functions_Group3 Attributes management functions
  *  @brief Attributes management functions.
  *
@verbatim
 ===============================================================================
                       ##### RCC attributes functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to:

    (+) Configure the RCC item(s) attribute(s).
    (+) Get the attribute of an RCC item.

@endverbatim
  * @{
  */

/**
  * @brief  Configure the RCC item(s) attribute(s).
  * @note   Available attributes are to control the secure, privileged, public and lock access rights of items.
  *         Default access rights are non-secure, unprivileged, non-public and unlocked.
  * @note   Secure/non-secure, privileged/unprivileged and public/non-public attributes can be set and reset
  *         from the secure/privileged state only.
  * @note   Lock Item attribute can be set from the secure/privileged state only.
  * @param  Item Item(s) to set attributes on.
  *         This parameter can be a one value or a combination of @ref RCC_items belonging to the same group of items
  *         (eg. RCC_ITEM_GROUP_OSC)
  * @param  Attributes specifies the RCC secure/privilege/public attributes.
  *         This parameter can be a combination of @ref RCC_attributes
  * @retval None
  *
  */
void HAL_RCC_ConfigAttributes(uint32_t Item, uint32_t Attributes)
{
  /* Check the parameters */
  assert_param(IS_RCC_ATTRIBUTES(Attributes));
  assert_param(IS_RCC_ITEM_ATTRIBUTES(Item));

  /* Check then configure items groups */
  /* Oscillators group */
  if ((Item & RCC_ITEM_GROUP_OSC) == RCC_ITEM_GROUP_OSC)
  {
    RCC_ATTR_ConfigItemGroup(RCC_ITEM_GROUP_IDX_OSC, Item & RCC_ITEM_GROUP_OSC_MASK, Attributes);
  }

  /* PLLs group */
  if ((Item & RCC_ITEM_GROUP_PLL) == RCC_ITEM_GROUP_PLL)
  {
    RCC_ATTR_ConfigItemGroup(RCC_ITEM_GROUP_IDX_PLL, Item & RCC_ITEM_GROUP_PLL_MASK, Attributes);
  }

  /* ICxs group */
  if ((Item & RCC_ITEM_GROUP_IC) == RCC_ITEM_GROUP_IC)
  {
    RCC_ATTR_ConfigItemGroup(RCC_ITEM_GROUP_IDX_IC, Item & RCC_ITEM_GROUP_IC_MASK, Attributes);
  }

  /* System configs group */
  if ((Item & RCC_ITEM_GROUP_SYSCFG) == RCC_ITEM_GROUP_SYSCFG)
  {
    RCC_ATTR_ConfigItemGroup(RCC_ITEM_GROUP_IDX_SYSCFG, Item & RCC_ITEM_GROUP_SYSCFG_MASK, Attributes);
  }

  /* Buses group */
  if ((Item & RCC_ITEM_GROUP_BUS) == RCC_ITEM_GROUP_BUS)
  {
    RCC_ATTR_ConfigItemGroup(RCC_ITEM_GROUP_IDX_BUS, Item & RCC_ITEM_GROUP_BUS_MASK, Attributes);
  }

  /* Memories group */
  if ((Item & RCC_ITEM_GROUP_MEM) == RCC_ITEM_GROUP_MEM)
  {
    RCC_ATTR_ConfigItemGroup(RCC_ITEM_GROUP_IDX_MEM, Item & RCC_ITEM_GROUP_MEM_MASK, Attributes);
  }
}

/**
  * @brief  Get the attributes of an RCC item.
  * @note   Secured items are available from non-secure state when set as public
  * @param  Item A single item to get secure/non-secure, public/non-public, privilege/non-privilege and locked
        attributes from. This parameter can be a value of RCC_items.
  * @param  pAttributes pointer to return the attributes (this is a combination of @ref RCC_attributes).
  * @retval HAL Status.
  *
  */
HAL_StatusTypeDef HAL_RCC_GetConfigAttributes(uint32_t Item, uint32_t *pAttributes)
{
  uint32_t attributes = 0UL;

  /* Check null pointer */
  if (pAttributes == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_RCC_SINGLE_ITEM_ATTRIBUTES(Item));

  /* Get the item group index and compute the attribute base register address*/
  uint32_t ItemGroup = (Item & RCC_ITEM_GROUP_MASK) >> RCC_ITEM_GROUP_POS;
  uint32_t ItemGroupIdx = 0UL;
  while ((ItemGroup >> ItemGroupIdx) != 1UL)
  {
    ItemGroupIdx++;
  }

  __IO const uint32_t *p_rcc_reg = &(RCC->SECCFGR0) + (0x4UL * ItemGroupIdx);

  if ((Item & RCC_ITEM_GROUP_MASK) != RCC_ITEM_GROUP_MEM)
  {
    /* Get secure attribute */
    attributes |= ((p_rcc_reg[0] & Item) == 0U) ? RCC_ATTR_NSEC : RCC_ATTR_SEC;

    /* Get privilege attribute */
    attributes |= ((p_rcc_reg[1] & Item) == 0U) ? RCC_ATTR_NPRIV : RCC_ATTR_PRIV;

    /* Get lock attribute */
    attributes |= ((p_rcc_reg[2] & Item) == 0U) ? RCC_ATTR_NLOCK : RCC_ATTR_LOCK;

    /* Get public attribute */
    attributes |= ((p_rcc_reg[3] & Item) == 0U) ? RCC_ATTR_NPUB : RCC_ATTR_PUB;
  }
  else
  {
    /* For memory group, only public attribute is available */
    attributes |= ((*p_rcc_reg & Item) == 0U) ? RCC_ATTR_NPUB : RCC_ATTR_PUB;
  }

  /* Return value */
  *pAttributes = attributes;

  return HAL_OK;
}

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup RCC_Private_functions RCC Private Functions
  * @{
  */
/**
  * @brief  Configure the requested PLL
  * @param  PLLnumber PLL number to configure
  * @param  pPLLInit Pointer to an RCC_PLLInitTypeDef structure that
  *                  contains the configuration parameters.
  * @note   PLL is temporary disabled to apply new parameters
  *
  * @retval HAL status
  */
static HAL_StatusTypeDef RCC_PLL_Config(uint32_t PLLnumber, const RCC_PLLInitTypeDef *pPLLInit)
{
  __IO uint32_t *p_rcc_pll_cfgr1_reg;
  __IO uint32_t *p_rcc_pll_cfgr2_reg;
  __IO uint32_t *p_rcc_pll_cfgr3_reg;
  HAL_StatusTypeDef ret = HAL_OK;
  uint32_t tickstart;

  p_rcc_pll_cfgr1_reg = &(RCC->PLL1CFGR1) + (((uint32_t)0x4) * PLLnumber);
  p_rcc_pll_cfgr2_reg = &(RCC->PLL1CFGR2) + (((uint32_t)0x4) * PLLnumber);
  p_rcc_pll_cfgr3_reg = &(RCC->PLL1CFGR3) + (((uint32_t)0x4) * PLLnumber);

  /* !!! WARNING: ONLY INTEGER AND FRACTIONAL MODES MANAGED TODAY !!! */
  if (pPLLInit->PLLState == RCC_PLL_ON)
  {
    /* Check the parameters */
    assert_param(IS_RCC_PLLSOURCE(pPLLInit->PLLSource));
    assert_param(IS_RCC_PLLFRACN_VALUE(pPLLInit->PLLFractional));
    assert_param(IS_RCC_PLLM_VALUE(pPLLInit->PLLM));
    assert_param(IS_RCC_PLLN_VALUE(pPLLInit->PLLN));
    assert_param(IS_RCC_PLLP_VALUE(pPLLInit->PLLP1));
    assert_param(IS_RCC_PLLP_VALUE(pPLLInit->PLLP2));

    /* Ensure PLLx is disabled */
    WRITE_REG(RCC->CCR, RCC_CCR_PLL1ONC << PLLnumber);

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    /* Wait till PLLx is disabled */
    while (READ_BIT(RCC->SR, (RCC_SR_PLL1RDY << PLLnumber)) == (RCC_SR_PLL1RDY << PLLnumber))
    {
      if ((HAL_GetTick() - tickstart) > RCC_PLL_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }

    /* Ensure PLLxMODSSDIS='1' */
    SET_BIT(*p_rcc_pll_cfgr3_reg, RCC_PLL1CFGR3_PLL1MODSSDIS);

    /* Clear bypass mode */
    CLEAR_BIT(*p_rcc_pll_cfgr1_reg, RCC_PLL1CFGR1_PLL1BYP);

    /* Configure the PLLx clock source, multiplication and division factors. */
    MODIFY_REG(*p_rcc_pll_cfgr1_reg, (RCC_PLL1CFGR1_PLL1SEL | RCC_PLL1CFGR1_PLL1DIVM | RCC_PLL1CFGR1_PLL1DIVN), \
               (pPLLInit->PLLSource | (pPLLInit->PLLM << RCC_PLL1CFGR1_PLL1DIVM_Pos) \
                | (pPLLInit->PLLN << RCC_PLL1CFGR1_PLL1DIVN_Pos)));
    MODIFY_REG(*p_rcc_pll_cfgr3_reg, (RCC_PLL1CFGR3_PLL1PDIV1 | RCC_PLL1CFGR3_PLL1PDIV2), \
               ((pPLLInit->PLLP1 << RCC_PLL1CFGR3_PLL1PDIV1_Pos) | (pPLLInit->PLLP2 << RCC_PLL1CFGR3_PLL1PDIV2_Pos)));

    /* Configure PLLx DIVNFRAC */
    MODIFY_REG(*p_rcc_pll_cfgr2_reg, RCC_PLL1CFGR2_PLL1DIVNFRAC, \
               pPLLInit->PLLFractional << RCC_PLL1CFGR2_PLL1DIVNFRAC_Pos);

    /* Clear PLLxMODDSEN (Also clear in Fractional Mode to ensure the latch of updated FRAC value when set again) */
    CLEAR_BIT(*p_rcc_pll_cfgr3_reg, RCC_PLL1CFGR3_PLL1MODDSEN);

    /* Fractional Mode specificities Management */
    if (pPLLInit->PLLFractional != 0U)
    {
      /* Set PLLxMODDSEN and DACEN */
      SET_BIT(*p_rcc_pll_cfgr3_reg, (RCC_PLL1CFGR3_PLL1MODDSEN | RCC_PLL1CFGR3_PLL1DACEN));
    }

    /* Ensure PLLxMODSSRST='1' and Enable PLLx post divider output */
    SET_BIT(*p_rcc_pll_cfgr3_reg, (RCC_PLL1CFGR3_PLL1MODSSRST | RCC_PLL1CFGR3_PLL1PDIVEN));

    /* Enable the PLLx */
    WRITE_REG(RCC->CSR, RCC_CSR_PLL1ONS << PLLnumber);

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    /* Wait till PLLx is ready */
    while (READ_BIT(RCC->SR, (RCC_SR_PLL1RDY << PLLnumber)) == 0U)
    {
      if ((HAL_GetTick() - tickstart) > RCC_PLL_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }
  }
  else if (pPLLInit->PLLState == RCC_PLL_BYPASS)
  {
    assert_param(IS_RCC_PLLSOURCE(pPLLInit->PLLSource));

    /* Check selected source is ready */
    if (RCC_PLL_Source_IsReady(pPLLInit->PLLSource) == 1U)
    {
      /* Ensure PLLx is disabled */
      WRITE_REG(RCC->CCR, RCC_CCR_PLL1ONC << PLLnumber);

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till PLLx is disabled */
      while (READ_BIT(RCC->SR, (RCC_SR_PLL1RDY << PLLnumber)) == (RCC_SR_PLL1RDY << PLLnumber))
      {
        if ((HAL_GetTick() - tickstart) > RCC_PLL_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }

      /* Set bypass mode with selected source */
      MODIFY_REG(*p_rcc_pll_cfgr1_reg, (RCC_PLL1CFGR1_PLL1BYP | RCC_PLL1CFGR1_PLL1SEL), \
                 (RCC_PLL1CFGR1_PLL1BYP | pPLLInit->PLLSource));
    }
    else
    {
      ret = HAL_ERROR;
    }
  }
  else if (pPLLInit->PLLState == RCC_PLL_OFF)
  {
    /* Disable PLLx post divider output */
    CLEAR_BIT(*p_rcc_pll_cfgr3_reg, RCC_PLL1CFGR3_PLL1PDIVEN);

    /* Ensure PLLx is disabled */
    WRITE_REG(RCC->CCR, RCC_CCR_PLL1ONC << PLLnumber);

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    /* Wait till PLLx is disabled */
    while (READ_BIT(RCC->SR, (RCC_SR_PLL1RDY << PLLnumber)) == (RCC_SR_PLL1RDY << PLLnumber))
    {
      if ((HAL_GetTick() - tickstart) > RCC_PLL_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }

    /* Clear bypass mode */
    CLEAR_BIT(*p_rcc_pll_cfgr1_reg, RCC_PLL1CFGR1_PLL1BYP);
  }
  else
  {
    /* Nothing to do */
  }

  return ret;
}

/**
  * @brief  Enable the requested PLL
  * @param  PLLnumber PLL number to enable
  *
  * @retval HAL status
  */
static HAL_StatusTypeDef RCC_PLL_Enable(uint32_t PLLnumber)
{
  HAL_StatusTypeDef ret = HAL_OK;
  uint32_t tickstart;

  /* Enable the PLLx */
  WRITE_REG(RCC->CSR, RCC_CSR_PLL1ONS << PLLnumber);

  /* Get Start Tick*/
  tickstart = HAL_GetTick();

  /* Wait till PLLx is ready */
  while (READ_BIT(RCC->SR, (RCC_SR_PLL1RDY << PLLnumber)) == 0U)
  {
    if ((HAL_GetTick() - tickstart) > RCC_PLL_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return ret;
}

/**
  * @brief  Check for a new PLL configuration
  * @param  PLLnumber PLL number
  * @param  pPLLInit Pointer to an RCC_PLLInitTypeDef structure that
  *                  contains the configuration parameters.  *
  * @retval 1 if success else 0
  */
static uint32_t RCC_PLL_IsNewConfig(uint32_t PLLnumber, const RCC_PLLInitTypeDef *pPLLInit)
{
  __IO const uint32_t *p_rcc_pll_cfgr1_reg, *p_rcc_pll_cfgr2_reg, *p_rcc_pll_cfgr3_reg;
  uint32_t ret = 0U;

  /* No assert since done in calling function */

  p_rcc_pll_cfgr1_reg = &(RCC->PLL1CFGR1) + (((uint32_t)0x4) * PLLnumber);
  p_rcc_pll_cfgr2_reg = &(RCC->PLL1CFGR2) + (((uint32_t)0x4) * PLLnumber);
  p_rcc_pll_cfgr3_reg = &(RCC->PLL1CFGR3) + (((uint32_t)0x4) * PLLnumber);

  /* !!! WARNING: ONLY INTEGER AND FRACTIONAL MODES MANAGED TODAY !!! */

  /* Check for PLLCFGR1, PLLCFGR2 and PLLCFGR3 parameters updates */
  if ((*p_rcc_pll_cfgr1_reg & (RCC_PLL1CFGR1_PLL1SEL | RCC_PLL1CFGR1_PLL1DIVM | RCC_PLL1CFGR1_PLL1DIVN)) != \
      (pPLLInit->PLLSource | (pPLLInit->PLLM << RCC_PLL1CFGR1_PLL1DIVM_Pos) \
       | (pPLLInit->PLLN << RCC_PLL1CFGR1_PLL1DIVN_Pos)))
  {
    ret = 1U; /* New PLL configuration */
  }
  else if ((*p_rcc_pll_cfgr2_reg & RCC_PLL1CFGR2_PLL1DIVNFRAC) != \
           (pPLLInit->PLLFractional << RCC_PLL1CFGR2_PLL1DIVNFRAC_Pos))
  {
    ret = 1U; /* New PLL configuration */
  }
  else if ((*p_rcc_pll_cfgr3_reg & (RCC_PLL1CFGR3_PLL1PDIV1 | RCC_PLL1CFGR3_PLL1PDIV2)) != \
           ((pPLLInit->PLLP1 << RCC_PLL1CFGR3_PLL1PDIV1_Pos) | (pPLLInit->PLLP2 << RCC_PLL1CFGR3_PLL1PDIV2_Pos)))
  {
    ret = 1U; /* New PLL configuration */
  }
  else
  {
    /* Mode change detection*/
    uint32_t pllState;

    /* Get current Mode*/
    if (READ_BIT(RCC->SR, (RCC_SR_PLL1RDY << PLLnumber)) == (RCC_SR_PLL1RDY << PLLnumber))
    {
      pllState = RCC_PLL_ON;
    }
    else
    {
      if ((*p_rcc_pll_cfgr1_reg & RCC_PLL1CFGR1_PLL1BYP) != 0UL)
      {
        pllState = RCC_PLL_BYPASS;
      }
      else
      {
        pllState = RCC_PLL_OFF;
      }
    }

    /* Compare with new mode */
    if (pllState != pPLLInit->PLLState)
    {
      ret = 1U; /* New PLL configuration */
    }
  }

  return ret;
}

/**
  * @brief  Check whether the PLL source is ready
  * @param  PLLSource PLL source
  * @retval 1 if success else 0
  */
static uint32_t RCC_PLL_Source_IsReady(uint32_t PLLSource)
{
  uint32_t ret = 1U;

  /* No assert since done in calling function */

  switch (PLLSource)
  {
    case RCC_PLLSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() == 0U)
      {
        ret = 0U;
      }
      break;
    case RCC_PLLSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() == 0U)
      {
        ret = 0U;
      }
      break;
    case RCC_PLLSOURCE_HSE:
      if (LL_RCC_HSE_IsReady() == 0U)
      {
        ret = 0U;
      }
      break;
    case RCC_PLLSOURCE_PIN:
    default:
      break;
  }

  return ret;
}

/**
  * @brief  Check whether PLL sources are available
  * @param  PLLSource1 First PLL source
  * @param  PLLSource2 Second PLL source
  * @retval 1 if success else 0
  */
static uint32_t RCC_IC_CheckPLLSources(uint32_t PLLSource1, uint32_t PLLSource2)
{
  uint32_t ret = 1U;

  /* No assert since done in calling function */

  /* Check PLLSource1 clock source */
  switch (PLLSource1)
  {
    case LL_RCC_ICCLKSOURCE_PLL1:
      if (LL_RCC_PLL1_IsReady() == 0U)
      {
        if (LL_RCC_PLL1_IsEnabledBypass() == 0U)
        {
          ret = 0U;
        }
      }
      break;
    case LL_RCC_ICCLKSOURCE_PLL2:
      if (LL_RCC_PLL2_IsReady() == 0U)
      {
        if (LL_RCC_PLL2_IsEnabledBypass() == 0U)
        {
          ret = 0U;
        }
      }
      break;
    case LL_RCC_ICCLKSOURCE_PLL3:
      if (LL_RCC_PLL3_IsReady() == 0U)
      {
        if (LL_RCC_PLL3_IsEnabledBypass() == 0U)
        {
          ret = 0U;
        }
      }
      break;
    case LL_RCC_ICCLKSOURCE_PLL4:
      if (LL_RCC_PLL4_IsReady() == 0U)
      {
        if (LL_RCC_PLL4_IsEnabledBypass() == 0U)
        {
          ret = 0U;
        }
      }
      break;
    default:
      /* Unexpected */
      ret = 0U;
      break;
  }

  /* Check PLLSource2 clock source */
  switch (PLLSource2)
  {
    case LL_RCC_ICCLKSOURCE_PLL1:
      if (LL_RCC_PLL1_IsReady() == 0U)
      {
        if (LL_RCC_PLL1_IsEnabledBypass() == 0U)
        {
          ret = 0U;
        }
      }
      break;
    case LL_RCC_ICCLKSOURCE_PLL2:
      if (LL_RCC_PLL2_IsReady() == 0U)
      {
        if (LL_RCC_PLL2_IsEnabledBypass() == 0U)
        {
          ret = 0U;
        }
      }
      break;
    case LL_RCC_ICCLKSOURCE_PLL3:
      if (LL_RCC_PLL3_IsReady() == 0U)
      {
        if (LL_RCC_PLL3_IsEnabledBypass() == 0U)
        {
          ret = 0U;
        }
      }
      break;
    case LL_RCC_ICCLKSOURCE_PLL4:
      if (LL_RCC_PLL4_IsReady() == 0U)
      {
        if (LL_RCC_PLL4_IsEnabledBypass() == 0U)
        {
          ret = 0U;
        }
      }
      break;
    default:
      /* Unexpected */
      ret = 0U;
      break;
  }

  return ret;
}

/**
  * @brief  Attribute configuration of a group of Items.
  * @param  ItemGroupIdx the Item group Index to configure
  * @param  Item Item(s) to set attributes on.
  *         This parameter can be a one or a combination of @ref RCC_items, masked with RCC_ITEM_MASK
  * @param  Attributes specifies the RCC secure/privilege/public/lock attributes.
  *         This parameter can be a combination of @ref RCC_attributes
  * @retval None
  */
static void RCC_ATTR_ConfigItemGroup(uint32_t ItemGroupIdx, uint32_t Item, uint32_t Attributes)
{
  __IO uint32_t *p_rcc_reg = &(RCC->SECCFGR0) + (0x4UL * ItemGroupIdx);

  if (ItemGroupIdx != RCC_ITEM_GROUP_IDX_MEM)
  {
#if defined (CPU_IN_SECURE_STATE)
    /* Check item security attribute management */
    if ((Attributes & RCC_ATTR_SEC_MASK) == RCC_ATTR_SEC_MASK)
    {
      /* Configure item security attribute */
      if ((Attributes & RCC_ATTR_SEC) == RCC_ATTR_SEC)
      {
        SET_BIT(p_rcc_reg[0], Item);
      }
      else
      {
        CLEAR_BIT(p_rcc_reg[0], Item);
      }
    }
#endif /* CPU_IN_SECURE_STATE */

    /* Check item privilege attribute management */
    if ((Attributes & RCC_ATTR_PRIV_MASK) == RCC_ATTR_PRIV_MASK)
    {
      /* Configure item privilege attribute */
      if ((Attributes & RCC_ATTR_PRIV) == RCC_ATTR_PRIV)
      {
        SET_BIT(p_rcc_reg[1], Item);
      }
      else
      {
        CLEAR_BIT(p_rcc_reg[1], Item);
      }
    }

#if defined (CPU_IN_SECURE_STATE)
    /* Check item lock attribute management */
    if ((Attributes & RCC_ATTR_LOCK_MASK) == RCC_ATTR_LOCK_MASK)
    {
      /* Configure item lock attribute */
      if ((Attributes & RCC_ATTR_LOCK) == RCC_ATTR_LOCK)
      {
        SET_BIT(p_rcc_reg[2], Item);
      }
    }

    /* Check item public attribute management */
    if ((Attributes & RCC_ATTR_PUB_MASK) == RCC_ATTR_PUB_MASK)
    {
      /* Configure item public attribute */
      if ((Attributes & RCC_ATTR_PUB) == RCC_ATTR_PUB)
      {
        SET_BIT(p_rcc_reg[3], Item);
      }
      else
      {
        CLEAR_BIT(p_rcc_reg[3], Item);
      }
    }
#endif /* CPU_IN_SECURE_STATE */
  }
  else
  {
    /* For memory group, only public attribute is available */
#if defined (CPU_IN_SECURE_STATE)
    /* Check item public attribute management */
    if ((Attributes & RCC_ATTR_PUB_MASK) == RCC_ATTR_PUB_MASK)
    {
      /* Configure item public attribute */
      if ((Attributes & RCC_ATTR_PUB) == RCC_ATTR_PUB)
      {
        SET_BIT(*(p_rcc_reg), Item);
      }
      else
      {
        CLEAR_BIT(*(p_rcc_reg), Item);
      }
    }
#endif /* CPU_IN_SECURE_STATE */
  }
}

/**
  * @}
  */

#endif /* HAL_RCC_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */
