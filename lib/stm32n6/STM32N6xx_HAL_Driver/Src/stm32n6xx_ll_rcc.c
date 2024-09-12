/**
  ******************************************************************************
  * @file    stm32n6xx_ll_rcc.c
  * @author  MCD Application Team
  * @brief   RCC LL module driver.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_ll_rcc.h"
#include "stm32n6xx_ll_bus.h"
#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined(RCC)

/** @addtogroup RCC_LL
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @addtogroup RCC_LL_Private_Macros
  * @{
  */
#define IS_LL_RCC_ADC_CLKSOURCE(__VALUE__)    ((__VALUE__) == LL_RCC_ADC_CLKSOURCE)

#define IS_LL_RCC_ADF_CLKSOURCE(__VALUE__)    ((__VALUE__) == LL_RCC_ADF1_CLKSOURCE)

#define IS_LL_RCC_DCMIPP_CLKSOURCE(__VALUE__) ((__VALUE__) == LL_RCC_DCMIPP_CLKSOURCE)

#define IS_LL_RCC_ETH_CLKSOURCE(__VALUE__)    ((__VALUE__) == LL_RCC_ETH1_CLKSOURCE)

#define IS_LL_RCC_ETHPTP_CLKSOURCE(__VALUE__) ((__VALUE__) == LL_RCC_ETH1PTP_CLKSOURCE)

#define IS_LL_RCC_FDCAN_CLKSOURCE(__VALUE__)  ((__VALUE__) == LL_RCC_FDCAN_CLKSOURCE)

#define IS_LL_RCC_FMC_CLKSOURCE(__VALUE__)    ((__VALUE__) == LL_RCC_FMC_CLKSOURCE)

#define IS_LL_RCC_I2C_CLKSOURCE(__VALUE__)    (((__VALUE__) == LL_RCC_I2C1_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_I2C2_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_I2C3_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_I2C4_CLKSOURCE))

#define IS_LL_RCC_I3C_CLKSOURCE(__VALUE__)    (((__VALUE__) == LL_RCC_I3C1_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_I3C2_CLKSOURCE))

#define IS_LL_RCC_LPTIM_CLKSOURCE(__VALUE__)  (((__VALUE__) == LL_RCC_LPTIM1_CLKSOURCE)  \
                                               || ((__VALUE__) == LL_RCC_LPTIM2_CLKSOURCE)  \
                                               || ((__VALUE__) == LL_RCC_LPTIM3_CLKSOURCE)  \
                                               || ((__VALUE__) == LL_RCC_LPTIM4_CLKSOURCE)  \
                                               || ((__VALUE__) == LL_RCC_LPTIM5_CLKSOURCE))

#define IS_LL_RCC_LTDC_CLKSOURCE(__VALUE__)   ((__VALUE__) == LL_RCC_LTDC_CLKSOURCE)

#define IS_LL_RCC_MDF_CLKSOURCE(__VALUE__)    ((__VALUE__) == LL_RCC_MDF1_CLKSOURCE)

#define IS_LL_RCC_OTGPHY_CLKSOURCE(__VALUE__) (((__VALUE__) == LL_RCC_OTGPHY1_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_OTGPHY2_CLKSOURCE))

#define IS_LL_RCC_OTGPHYCKREF_CLKSOURCE(__VALUE__) (((__VALUE__) == LL_RCC_OTGPHY1CKREF_CLKSOURCE) \
                                                    || ((__VALUE__) == LL_RCC_OTGPHY2CKREF_CLKSOURCE))

#define IS_LL_RCC_PSSI_CLKSOURCE(__VALUE__)   ((__VALUE__) == LL_RCC_PSSI_CLKSOURCE)

#define IS_LL_RCC_SAI_CLKSOURCE(__VALUE__)    (((__VALUE__) == LL_RCC_SAI1_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_SAI2_CLKSOURCE))

#define IS_LL_RCC_SDMMC_CLKSOURCE(__VALUE__)  (((__VALUE__) == LL_RCC_SDMMC1_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_SDMMC2_CLKSOURCE))

#define IS_LL_RCC_SPDIFRX_CLKSOURCE(__VALUE__) ((__VALUE__) == LL_RCC_SPDIFRX1_CLKSOURCE)

#define IS_LL_RCC_SPI_CLKSOURCE(__VALUE__)    (((__VALUE__) == LL_RCC_SPI1_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_SPI2_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_SPI3_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_SPI4_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_SPI5_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_SPI6_CLKSOURCE))

#define IS_LL_RCC_UART_CLKSOURCE(__VALUE__)   (((__VALUE__) == LL_RCC_UART4_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_UART5_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_UART7_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_UART8_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_UART9_CLKSOURCE))

#define IS_LL_RCC_USART_CLKSOURCE(__VALUE__)  (((__VALUE__) == LL_RCC_USART1_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_USART2_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_USART3_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_USART6_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_USART10_CLKSOURCE))

#define IS_LL_RCC_XSPI_CLKSOURCE(__VALUE__)   (((__VALUE__) == LL_RCC_XSPI1_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_XSPI2_CLKSOURCE) \
                                               || ((__VALUE__) == LL_RCC_XSPI3_CLKSOURCE))

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup RCC_LL_Private_Functions RCC Private functions
  * @{
  */
static uint32_t RCC_GetHCLKClockFreq(uint32_t SYSCLK_Frequency);
static uint32_t RCC_GetPCLK1ClockFreq(uint32_t HCLK_Frequency);
static uint32_t RCC_GetPCLK2ClockFreq(uint32_t HCLK_Frequency);
static uint32_t RCC_GetPCLK4ClockFreq(uint32_t HCLK_Frequency);
static uint32_t RCC_GetPCLK5ClockFreq(uint32_t HCLK_Frequency);

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @addtogroup RCC_LL_Exported_Functions
  * @{
  */

/** @addtogroup RCC_LL_EF_Init
  * @{
  */

/**
  * @brief  Resets the RCC clock configuration to the default reset state.
  * @note   The default reset state of the clock configuration is given below:
  *            - HSI ON and used as system clock source
  *            - HSE, PLL1, PLL2 and PLL3 OFF
  *            - AHB, APB Bus pre-scaler set to 1.
  *            - CSS, MCO1 and MCO2 OFF
  *            - All interrupts disabled
  * @note   This function doesn't modify the configuration of the
  *            - Peripheral clocks
  *            - LSI, LSE and RTC clocks
  * @retval None
  */
void LL_RCC_DeInit(void)
{
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

  /* Wait for HSI READY bit */
  while (LL_RCC_HSI_IsReady() == 0U)
  {}

  /* Reset CFGR1 register to select HSI as system clock */
  CLEAR_REG(RCC->CFGR1);

  /* Reset MSION, HSEON, PLL1ON, PLL2ON, PLL3ON and PLL4ON bits */
  WRITE_REG(RCC->CCR, RCC_CCR_MSIONC | RCC_CCR_HSEONC | \
            RCC_CCR_PLL1ONC | RCC_CCR_PLL2ONC | RCC_CCR_PLL3ONC | RCC_CCR_PLL4ONC);

  /* Wait for PLL1 READY bit to be reset */
  while (LL_RCC_PLL1_IsReady() != 0U)
  {}

  /* Wait for PLL2 READY bit to be reset */
  while (LL_RCC_PLL2_IsReady() != 0U)
  {}

  /* Wait for PLL3 READY bit to be reset */
  while (LL_RCC_PLL3_IsReady() != 0U)
  {}

  /* Wait for PLL4 READY bit to be reset */
  while (LL_RCC_PLL4_IsReady() != 0U)
  {}

  /* Reset prescalers in CFGR2 register */
  CLEAR_REG(RCC->CFGR2);

  /* Reset LSECFGR register */
  CLEAR_REG(RCC->LSECFGR);

  /* Reset MSICFGR register */
  CLEAR_REG(RCC->MSICFGR);

  /* Reset HSIMCR register to default value */
  WRITE_REG(RCC->HSIMCR, 0x001F07A1U);

  /* Reset HSECFGR register to default value */
  WRITE_REG(RCC->HSECFGR, 0x00000800U);

  /* Reset STOPCR register to default value */
  WRITE_REG(RCC->STOPCR, 0x00000008U);

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

  /* Disable DIVENR register to set ICx state to default value */
  WRITE_REG(RCC->DIVENCR, 0x00FFFFFFU);

  /* ICxCFGR reset value */
  WRITE_REG(RCC->IC1CFGR, 0x00020000U);
  WRITE_REG(RCC->IC2CFGR, 0x00030000U);
  WRITE_REG(RCC->IC3CFGR, 0x00000000U);
  WRITE_REG(RCC->IC4CFGR, 0x00000000U);
  WRITE_REG(RCC->IC5CFGR, 0x00000000U);
  WRITE_REG(RCC->IC6CFGR, 0x00030000U);
  WRITE_REG(RCC->IC7CFGR, 0x10000000U);
  WRITE_REG(RCC->IC8CFGR, 0x10000000U);
  WRITE_REG(RCC->IC9CFGR, 0x10000000U);
  WRITE_REG(RCC->IC10CFGR, 0x10000000U);
  WRITE_REG(RCC->IC11CFGR, 0x00030000U);
  WRITE_REG(RCC->IC12CFGR, 0x20000000U);
  WRITE_REG(RCC->IC13CFGR, 0x20000000U);
  WRITE_REG(RCC->IC14CFGR, 0x20000000U);
  WRITE_REG(RCC->IC15CFGR, 0x20000000U);
  WRITE_REG(RCC->IC16CFGR, 0x30000000U);
  WRITE_REG(RCC->IC17CFGR, 0x30000000U);
  WRITE_REG(RCC->IC18CFGR, 0x30000000U);
  WRITE_REG(RCC->IC19CFGR, 0x30000000U);
  WRITE_REG(RCC->IC20CFGR, 0x30000000U);

  /* CCIPRx reset value except RTCSEL/RTCPRE preserved */
  WRITE_REG(RCC->CCIPR1, 0x00000000U);
  WRITE_REG(RCC->CCIPR2, 0x00000000U);
  WRITE_REG(RCC->CCIPR3, 0x00000001U);
  WRITE_REG(RCC->CCIPR4, 0x00000000U);
  WRITE_REG(RCC->CCIPR5, 0x0000F0F0U);
  WRITE_REG(RCC->CCIPR6, 0x00000000U);
  MODIFY_REG(RCC->CCIPR7, RCC_CCIPR7_PERSEL | RCC_CCIPR7_PSSISEL | \
             RCC_CCIPR7_SAI1SEL | RCC_CCIPR7_SAI2SEL, 0x00000000U);
  WRITE_REG(RCC->CCIPR8, 0x00000000U);
  WRITE_REG(RCC->CCIPR9, 0x00000000U);
  WRITE_REG(RCC->CCIPR12, 0x00000000U);
  WRITE_REG(RCC->CCIPR13, 0x00000000U);
  WRITE_REG(RCC->CCIPR14, 0x00000000U);

  /* Reset MEMENR register to default value */
  WRITE_REG(RCC->MEMENR, 0x000013FFU);

  /* Reset MISCENR register */
  CLEAR_REG(RCC->MISCENR);
}

/**
  * @}
  */

/** @addtogroup RCC_LL_EF_Get_Freq
  * @brief  Return the frequencies of different on chip clocks;  System, AHB, APB1, APB2, APB3 and APB4 buses clocks.
  *         and different peripheral clocks available on the device.
  * @note   If SYSCLK source is HSI, function returns values based on HSI_VALUE(*)
  * @note   If SYSCLK source is HSE, function returns values based on HSE_VALUE(**)
  * @note   If SYSCLK source is MSI, function returns values based on MSI_VALUE(***)
  * @note   If SYSCLK source is PLL, function returns values based on HSE_VALUE(**)
  *         or HSI_VALUE(*) multiplied/divided by the PLL factors.
  * @note   (*) HSI_VALUE is a constant defined in header file (default value
  *             64 MHz) divider by HSIDIV, but the real value may vary depending on
  *             on the variations in voltage and temperature.
  * @note   (**) HSE_VALUE is a constant defined in header file (default value
  *              25 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  * @note   (***) MSI_VALUE is a constant defined in header file (default value
  *               4 MHz) but the real value may vary depending on the variations
  *               in voltage and temperature.
  * @note   The result of this function could be incorrect when using fractional
  *         value for HSE crystal.
  * @note   This function can be used by the user application to compute the
  *         baud-rate for the communication peripherals or configure other parameters.
  * @{
  */

/**
  * @brief  Return the frequencies of different on chip clocks;  CPU, System bus, AHB, APB1, APB2, APB4
  *         and APB5 buses clocks.
  * @note   Each time CPUCLK, SYSCLK, HCLK, PCLK1, PCLK2, PCLK4 and/or PCLK5 clock changes, this function
  *         must be called to update application structure fields. Otherwise, any configuration based on this
  *         function will be incorrect.
  * @param  RCC_Clocks pointer to a @ref LL_RCC_ClocksTypeDef structure which will hold the clocks frequencies
  * @retval None
  */
void LL_RCC_GetSystemClocksFreq(LL_RCC_ClocksTypeDef *RCC_Clocks)
{
  /* Get CPU frequency */
  RCC_Clocks->CPUCLK_Frequency = LL_RCC_GetCpuClockFreq();

  /* Get SYSCLK frequency */
  RCC_Clocks->SYSCLK_Frequency = LL_RCC_GetSystemClockFreq();

  /* HCLK clock frequency */
  RCC_Clocks->HCLK_Frequency   = RCC_GetHCLKClockFreq(RCC_Clocks->SYSCLK_Frequency);

  /* PCLK1 clock frequency */
  RCC_Clocks->PCLK1_Frequency  = RCC_GetPCLK1ClockFreq(RCC_Clocks->HCLK_Frequency);

  /* PCLK2 clock frequency */
  RCC_Clocks->PCLK2_Frequency  = RCC_GetPCLK2ClockFreq(RCC_Clocks->HCLK_Frequency);

  /* PCLK4 clock frequency */
  RCC_Clocks->PCLK4_Frequency  = RCC_GetPCLK4ClockFreq(RCC_Clocks->HCLK_Frequency);

  /* PCLK5 clock frequency */
  RCC_Clocks->PCLK5_Frequency  = RCC_GetPCLK5ClockFreq(RCC_Clocks->HCLK_Frequency);
}

/**
  * @brief  Return CPU clock frequency
  * @retval CPU clock frequency (in Hz)
  */
uint32_t LL_RCC_GetCpuClockFreq(void)
{
  uint32_t frequency = 0U;
  uint32_t ic_divider;

  /* Get CPU clock source ----------------------------------------------------*/
  switch (LL_RCC_GetCpuClkSource())
  {
    /* No check on Ready: Won't be selected by hardware if not */
    case LL_RCC_CPU_CLKSOURCE_STATUS_HSI:
      frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      break;

    case LL_RCC_CPU_CLKSOURCE_STATUS_MSI:
      frequency = MSI_VALUE;
      break;

    case LL_RCC_CPU_CLKSOURCE_STATUS_HSE:
      frequency = HSE_VALUE;
      break;

    case LL_RCC_CPU_CLKSOURCE_STATUS_IC1:
      ic_divider = LL_RCC_IC1_GetDivider();
      switch (LL_RCC_IC1_GetSource())
      {
        case LL_RCC_ICCLKSOURCE_PLL1:
          frequency = LL_RCC_GetPLL1ClockFreq();
          frequency = frequency / ic_divider;
          break;
        case LL_RCC_ICCLKSOURCE_PLL2:
          frequency = LL_RCC_GetPLL2ClockFreq();
          frequency = frequency / ic_divider;
          break;
        case LL_RCC_ICCLKSOURCE_PLL3:
          frequency = LL_RCC_GetPLL3ClockFreq();
          frequency = frequency / ic_divider;
          break;
        case LL_RCC_ICCLKSOURCE_PLL4:
          frequency = LL_RCC_GetPLL4ClockFreq();
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
  * @brief  Return SYSTEM bus clock frequency
  * @retval SYSTEM bus clock frequency (in Hz)
  */
uint32_t LL_RCC_GetSystemClockFreq(void)
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
      frequency = MSI_VALUE;
      break;

    case LL_RCC_SYS_CLKSOURCE_STATUS_HSE:
      frequency = HSE_VALUE;
      break;

    case LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11:
      ic_divider = LL_RCC_IC2_GetDivider();
      switch (LL_RCC_IC2_GetSource())
      {
        case LL_RCC_ICCLKSOURCE_PLL1:
          frequency = LL_RCC_GetPLL1ClockFreq();
          frequency = frequency / ic_divider;
          break;
        case LL_RCC_ICCLKSOURCE_PLL2:
          frequency = LL_RCC_GetPLL2ClockFreq();
          frequency = frequency / ic_divider;
          break;
        case LL_RCC_ICCLKSOURCE_PLL3:
          frequency = LL_RCC_GetPLL3ClockFreq();
          frequency = frequency / ic_divider;
          break;
        case LL_RCC_ICCLKSOURCE_PLL4:
          frequency = LL_RCC_GetPLL4ClockFreq();
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
  * @brief  Return PLL1 clock frequency
  * @note   LL_RCC_PERIPH_FREQUENCY_NO returned for non activated output or oscillator not ready
  * @retval PLL1 output clock frequency
  */
uint32_t LL_RCC_GetPLL1ClockFreq(void)
{
  uint32_t pllinputfreq = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t plloutputfreq = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t pllsource;
  uint32_t divm;

  /* PLL_VCO = (HSE_VALUE, MSI_VALUE or HSI_VALUE/HSIDIV) / PLLM * (PLLN + FRACN)
     SYSCLK = PLL_VCO / PLLP1 / PLLP2
  */
  if (LL_RCC_PLL1_IsReady() != 0U)
  {
    if (LL_RCC_PLL1P_IsEnabled() != 0U)
    {
      pllsource = LL_RCC_PLL1_GetSource();

      switch (pllsource)
      {
        case LL_RCC_PLLSOURCE_HSI:
          if (LL_RCC_HSI_IsReady() != 0U)
          {
            pllinputfreq = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
          }
          break;

        case LL_RCC_PLLSOURCE_MSI:
          if (LL_RCC_MSI_IsReady() != 0U)
          {
            pllinputfreq = MSI_VALUE;
          }
          break;

        case LL_RCC_PLLSOURCE_HSE:
          if (LL_RCC_HSE_IsReady() != 0U)
          {
            pllinputfreq = HSE_VALUE;
          }
          break;

        case LL_RCC_PLLSOURCE_I2S_CKIN:
          pllinputfreq = EXTERNAL_CLOCK_VALUE;
          break;

        default:
          /* unexpected case */
          break;
      }

      if (pllinputfreq != LL_RCC_PERIPH_FREQUENCY_NO)
      {
        /* INTEGER mode only  - others TO DO */
        divm = LL_RCC_PLL1_GetM();

        if (divm != 0U)
        {
          plloutputfreq = LL_RCC_CalcPLLClockFreq(pllinputfreq, divm, LL_RCC_PLL1_GetN(), LL_RCC_PLL1_GetFRACN(), \
                                                  LL_RCC_PLL1_GetP1(), LL_RCC_PLL1_GetP2());
        }
      }
    }
  }

  return plloutputfreq;
}

/**
  * @brief  Return PLL2 clock frequency
  * @note   LL_RCC_PERIPH_FREQUENCY_NO returned for non activated output or oscillator not ready
  * @retval PLL2 output clock frequency
  */
uint32_t LL_RCC_GetPLL2ClockFreq(void)
{
  uint32_t pllinputfreq = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t plloutputfreq = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t pllsource;
  uint32_t divm;

  /* PLL_VCO = (HSE_VALUE, MSI_VALUE or HSI_VALUE/HSIDIV) / PLLM * (PLLN + FRACN)
     SYSCLK = PLL_VCO / PLLP1 / PLLP2
  */
  if (LL_RCC_PLL2_IsReady() != 0U)
  {
    if (LL_RCC_PLL2P_IsEnabled() != 0U)
    {
      pllsource = LL_RCC_PLL2_GetSource();

      switch (pllsource)
      {
        case LL_RCC_PLLSOURCE_HSI:
          if (LL_RCC_HSI_IsReady() != 0U)
          {
            pllinputfreq = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
          }
          break;

        case LL_RCC_PLLSOURCE_MSI:
          if (LL_RCC_MSI_IsReady() != 0U)
          {
            pllinputfreq = MSI_VALUE;
          }
          break;

        case LL_RCC_PLLSOURCE_HSE:
          if (LL_RCC_HSE_IsReady() != 0U)
          {
            pllinputfreq = HSE_VALUE;
          }
          break;

        case LL_RCC_PLLSOURCE_I2S_CKIN:
          pllinputfreq = EXTERNAL_CLOCK_VALUE;
          break;

        default:
          /* unexpected case */
          break;
      }

      if (pllinputfreq != LL_RCC_PERIPH_FREQUENCY_NO)
      {
        /* INTEGER mode only  - others TO DO */
        divm = LL_RCC_PLL2_GetM();

        if (divm != 0U)
        {
          plloutputfreq = LL_RCC_CalcPLLClockFreq(pllinputfreq, divm, LL_RCC_PLL2_GetN(), LL_RCC_PLL2_GetFRACN(), \
                                                  LL_RCC_PLL2_GetP1(), LL_RCC_PLL2_GetP2());
        }
      }
    }
  }

  return plloutputfreq;
}

/**
  * @brief  Return PLL3 clock frequency
  * @note   LL_RCC_PERIPH_FREQUENCY_NO returned for non activated output or oscillator not ready
  * @retval PLL3 output clock frequency
  */
uint32_t LL_RCC_GetPLL3ClockFreq(void)
{
  uint32_t pllinputfreq = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t plloutputfreq = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t pllsource;
  uint32_t divm;

  /* PLL_VCO = (HSE_VALUE, MSI_VALUE or HSI_VALUE/HSIDIV) / PLLM * (PLLN + FRACN)
     SYSCLK = PLL_VCO / PLLP1 / PLLP2
  */
  if (LL_RCC_PLL3_IsReady() != 0U)
  {
    if (LL_RCC_PLL3P_IsEnabled() != 0U)
    {
      pllsource = LL_RCC_PLL3_GetSource();

      switch (pllsource)
      {
        case LL_RCC_PLLSOURCE_HSI:
          if (LL_RCC_HSI_IsReady() != 0U)
          {
            pllinputfreq = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
          }
          break;

        case LL_RCC_PLLSOURCE_MSI:
          if (LL_RCC_MSI_IsReady() != 0U)
          {
            pllinputfreq = MSI_VALUE;
          }
          break;

        case LL_RCC_PLLSOURCE_HSE:
          if (LL_RCC_HSE_IsReady() != 0U)
          {
            pllinputfreq = HSE_VALUE;
          }
          break;

        case LL_RCC_PLLSOURCE_I2S_CKIN:
          pllinputfreq = EXTERNAL_CLOCK_VALUE;
          break;

        default:
          /* unexpected case */
          break;
      }

      if (pllinputfreq != LL_RCC_PERIPH_FREQUENCY_NO)
      {
        /* INTEGER mode only  - others TO DO */
        divm = LL_RCC_PLL3_GetM();

        if (divm != 0U)
        {
          plloutputfreq = LL_RCC_CalcPLLClockFreq(pllinputfreq, divm, LL_RCC_PLL3_GetN(), LL_RCC_PLL3_GetFRACN(), \
                                                  LL_RCC_PLL3_GetP1(), LL_RCC_PLL3_GetP2());
        }
      }
    }
  }

  return plloutputfreq;
}

/**
  * @brief  Return PLL4 clock frequency
  * @note   LL_RCC_PERIPH_FREQUENCY_NO returned for non activated output or oscillator not ready
  * @retval PLL4 output clock frequency
  */
uint32_t LL_RCC_GetPLL4ClockFreq(void)
{
  uint32_t pllinputfreq = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t plloutputfreq = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t pllsource;
  uint32_t divm;

  /* PLL_VCO = (HSE_VALUE, MSI_VALUE or HSI_VALUE/HSIDIV) / PLLM * (PLLN + FRACN)
     SYSCLK = PLL_VCO / PLLP1 / PLLP2
  */
  if (LL_RCC_PLL4_IsReady() != 0U)
  {
    if (LL_RCC_PLL4P_IsEnabled() != 0U)
    {
      pllsource = LL_RCC_PLL4_GetSource();

      switch (pllsource)
      {
        case LL_RCC_PLLSOURCE_HSI:
          if (LL_RCC_HSI_IsReady() != 0U)
          {
            pllinputfreq = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
          }
          break;

        case LL_RCC_PLLSOURCE_MSI:
          if (LL_RCC_MSI_IsReady() != 0U)
          {
            pllinputfreq = MSI_VALUE;
          }
          break;

        case LL_RCC_PLLSOURCE_HSE:
          if (LL_RCC_HSE_IsReady() != 0U)
          {
            pllinputfreq = HSE_VALUE;
          }
          break;

        case LL_RCC_PLLSOURCE_I2S_CKIN:
          pllinputfreq = EXTERNAL_CLOCK_VALUE;
          break;

        default:
          /* unexpected case */
          break;
      }

      if (pllinputfreq != LL_RCC_PERIPH_FREQUENCY_NO)
      {
        /* INTEGER mode only  - others TO DO */
        divm = LL_RCC_PLL4_GetM();

        if (divm != 0U)
        {
          plloutputfreq = LL_RCC_CalcPLLClockFreq(pllinputfreq, divm, LL_RCC_PLL4_GetN(), LL_RCC_PLL4_GetFRACN(), \
                                                  LL_RCC_PLL4_GetP1(), LL_RCC_PLL4_GetP2());
        }
      }
    }
  }

  return plloutputfreq;
}

/**
  * @brief  Helper function to calculate the PLL frequency output when used in integer or fractional mode
  * @note ex: @ref LL_RCC_CalcPLLClockFreq (HSE_VALUE, @ref LL_RCC_PLL1_GetM (), @ref LL_RCC_PLL1_GetN (),
  *           @ref LL_RCC_PLL1_GetFRACN (), @ref LL_RCC_PLL1_GetP1 (), @ref LL_RCC_PLL1_GetP2 ());
  * @param  PLLInputFreq PLL Input frequency (based on HSE/(HSI/HSIDIV)/MSI)
  * @param  M      Between 1 and 63
  * @param  N      Between 16 and 640 in integer mode, 20 to 320 in fractional mode
  * @param  FRACN  0 in integer mode, between 0 and 0xFFFFFF in fractional mode
  * @param  P1     VCO output divider P1 between 1 and 7
  * @param  P2     VCO output divider P2 between 1 and 7
  * @retval PLL clock frequency (in Hz)
  */
uint32_t LL_RCC_CalcPLLClockFreq(uint32_t PLLInputFreq, uint32_t M, uint32_t N, uint32_t FRACN, uint32_t P1,
                                 uint32_t P2)
{
  float_t freq;

  freq = ((float_t)PLLInputFreq * ((float_t)N + ((float_t)FRACN / (float_t)0x1000000))) / (float_t)M;

  freq = freq / (float_t)P1;
  freq = freq / (float_t)P2;

  return (uint32_t)freq;
}

/**
  * @brief  Return ADC clock frequency
  * @param  ADCxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ADC_CLKSOURCE
  * @retval ADC clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetADCClockFreq(uint32_t ADCxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_ADC_CLKSOURCE(ADCxSource));

  switch (LL_RCC_GetADCClockSource(ADCxSource))
  {
    case LL_RCC_ADC_CLKSOURCE_HCLK:
      frequency = RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq());
      break;

    case LL_RCC_ADC_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_ADC_CLKSOURCE_IC7:
      if (LL_RCC_IC7_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC7_GetDivider();
        switch (LL_RCC_IC7_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_ADC_CLKSOURCE_IC8:
      if (LL_RCC_IC8_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC8_GetDivider();
        switch (LL_RCC_IC8_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_ADC_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_ADC_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_ADC_CLKSOURCE_I2S_CKIN:
      frequency = EXTERNAL_CLOCK_VALUE;
      break;

    case LL_RCC_ADC_CLKSOURCE_TIMG:
      frequency = LL_RCC_GetSystemClockFreq() / (1UL << LL_RCC_GetTIMPrescaler());
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return ADFx clock frequency
  * @param  ADFxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE
  * @retval ADF clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetADFClockFreq(uint32_t ADFxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_ADF_CLKSOURCE(ADFxSource));

  switch (LL_RCC_GetADFClockSource(ADFxSource))
  {
    case LL_RCC_ADF1_CLKSOURCE_HCLK:
      frequency = RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq());
      break;

    case LL_RCC_ADF1_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_ADF1_CLKSOURCE_IC7:
      if (LL_RCC_IC7_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC7_GetDivider();
        switch (LL_RCC_IC7_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_ADF1_CLKSOURCE_IC8:
      if (LL_RCC_IC8_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC8_GetDivider();
        switch (LL_RCC_IC8_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_ADF1_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_ADF1_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_ADF1_CLKSOURCE_I2S_CKIN:
      frequency = EXTERNAL_CLOCK_VALUE;
      break;

    case LL_RCC_ADF1_CLKSOURCE_TIMG:
      frequency = LL_RCC_GetSystemClockFreq() / (1UL << LL_RCC_GetTIMPrescaler());
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return CLKP clock frequency
  * @param  CLKPxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE
  * @retval CLKP clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetCLKPClockFreq(uint32_t CLKPxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetCLKPClockSource(CLKPxSource))
  {
    case LL_RCC_CLKP_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_CLKP_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_CLKP_CLKSOURCE_HSE:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        frequency = HSE_VALUE;
      }
      break;

    case LL_RCC_CLKP_CLKSOURCE_IC5:
      if (LL_RCC_IC5_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC5_GetDivider();
        switch (LL_RCC_IC5_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_CLKP_CLKSOURCE_IC10:
      if (LL_RCC_IC10_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC10_GetDivider();
        switch (LL_RCC_IC10_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_CLKP_CLKSOURCE_IC15:
      if (LL_RCC_IC15_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC15_GetDivider();
        switch (LL_RCC_IC15_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_CLKP_CLKSOURCE_IC19:
      if (LL_RCC_IC19_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC19_GetDivider();
        switch (LL_RCC_IC19_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_CLKP_CLKSOURCE_IC20:
      if (LL_RCC_IC20_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC20_GetDivider();
        switch (LL_RCC_IC20_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return DCMIPP clock frequency
  * @param  DCMIPPxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_DCMIPP_CLKSOURCE
  * @retval DCMIPP clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetDCMIPPClockFreq(uint32_t DCMIPPxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_DCMIPP_CLKSOURCE(DCMIPPxSource));

  switch (LL_RCC_GetDCMIPPClockSource(DCMIPPxSource))
  {
    case LL_RCC_DCMIPP_CLKSOURCE_PCLK5:
      frequency = RCC_GetPCLK5ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_DCMIPP_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_DCMIPP_CLKSOURCE_IC17:
      if (LL_RCC_IC17_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC17_GetDivider();
        switch (LL_RCC_IC17_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_DCMIPP_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return ETH clock frequency
  * @param  ETHxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1_CLKSOURCE
  * @retval ETH1 clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetETHClockFreq(uint32_t ETHxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_ETH_CLKSOURCE(ETHxSource));

  switch (LL_RCC_GetETHClockSource(ETHxSource))
  {
    case LL_RCC_ETH1_CLKSOURCE_HCLK:
      frequency = RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq());
      break;

    case LL_RCC_ETH1_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_ETH1_CLKSOURCE_IC12:
      if (LL_RCC_IC12_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC12_GetDivider();
        switch (LL_RCC_IC12_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_ETH1_CLKSOURCE_HSE:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        frequency = HSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return ETH PTP clock frequency
  * @param  ETHxPTPSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1PTP_CLKSOURCE
  * @retval ETH1 PTP clock frequency with divider (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetETHPTPClockFreq(uint32_t ETHxPTPSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_ETHPTP_CLKSOURCE(ETHxPTPSource));

  switch (LL_RCC_GetETHPTPClockSource(ETHxPTPSource))
  {
    case LL_RCC_ETH1PTP_CLKSOURCE_HCLK:
      frequency = RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq());
      break;

    case LL_RCC_ETH1PTP_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_ETH1PTP_CLKSOURCE_IC13:
      if (LL_RCC_IC13_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC13_GetDivider();
        switch (LL_RCC_IC13_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_ETH1PTP_CLKSOURCE_HSE:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        frequency = HSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  /* Apply divider */
  if (frequency != LL_RCC_PERIPH_FREQUENCY_NO)
  {
    frequency = frequency / ((LL_RCC_GetETH1PTPDivider() >> RCC_CCIPR2_ETH1PTPDIV_Pos) + 1U);
  }

  return frequency;
}

/**
  * @brief  Return FDCAN clock frequency
  * @param  FDCANxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_FDCAN_CLKSOURCE
  * @retval FDCAN clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetFDCANClockFreq(uint32_t FDCANxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetFDCANClockSource(FDCANxSource))
  {
    case LL_RCC_FDCAN_CLKSOURCE_PCLK1:
      frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_FDCAN_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_FDCAN_CLKSOURCE_IC19:
      if (LL_RCC_IC19_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC19_GetDivider();
        switch (LL_RCC_IC19_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_FDCAN_CLKSOURCE_HSE:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        frequency = HSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return FMC clock frequency
  * @param  FMCxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_FMC_CLKSOURCE
  * @retval FMC clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetFMCClockFreq(uint32_t FMCxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetFMCClockSource(FMCxSource))
  {
    case LL_RCC_FMC_CLKSOURCE_HCLK:
      frequency = RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq());
      break;

    case LL_RCC_FMC_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_FMC_CLKSOURCE_IC3:
      if (LL_RCC_IC3_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC3_GetDivider();
        switch (LL_RCC_IC3_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_FMC_CLKSOURCE_IC4:
      if (LL_RCC_IC4_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC4_GetDivider();
        switch (LL_RCC_IC4_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return I2Cx clock frequency
  * @param  I2CxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE
  * @retval I2C clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetI2CClockFreq(uint32_t I2CxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_I2C_CLKSOURCE(I2CxSource));

  switch (LL_RCC_GetI2CClockSource(I2CxSource))
  {
    case LL_RCC_I2C1_CLKSOURCE_PCLK1:
    case LL_RCC_I2C2_CLKSOURCE_PCLK1:
    case LL_RCC_I2C3_CLKSOURCE_PCLK1:
    case LL_RCC_I2C4_CLKSOURCE_PCLK1:
      frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_I2C1_CLKSOURCE_CLKP:
    case LL_RCC_I2C2_CLKSOURCE_CLKP:
    case LL_RCC_I2C3_CLKSOURCE_CLKP:
    case LL_RCC_I2C4_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_I2C1_CLKSOURCE_IC10:
    case LL_RCC_I2C2_CLKSOURCE_IC10:
    case LL_RCC_I2C3_CLKSOURCE_IC10:
    case LL_RCC_I2C4_CLKSOURCE_IC10:
      if (LL_RCC_IC10_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC10_GetDivider();
        switch (LL_RCC_IC10_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_I2C1_CLKSOURCE_IC15:
    case LL_RCC_I2C2_CLKSOURCE_IC15:
    case LL_RCC_I2C3_CLKSOURCE_IC15:
    case LL_RCC_I2C4_CLKSOURCE_IC15:
      if (LL_RCC_IC15_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC15_GetDivider();
        switch (LL_RCC_IC15_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_I2C1_CLKSOURCE_HSI:
    case LL_RCC_I2C2_CLKSOURCE_HSI:
    case LL_RCC_I2C3_CLKSOURCE_HSI:
    case LL_RCC_I2C4_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_I2C1_CLKSOURCE_MSI:
    case LL_RCC_I2C2_CLKSOURCE_MSI:
    case LL_RCC_I2C3_CLKSOURCE_MSI:
    case LL_RCC_I2C4_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        frequency = MSI_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return I3Cx clock frequency
  * @param  I3CxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE
  * @retval I3C clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetI3CClockFreq(uint32_t I3CxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_I3C_CLKSOURCE(I3CxSource));

  switch (LL_RCC_GetI3CClockSource(I3CxSource))
  {
    case LL_RCC_I3C1_CLKSOURCE_PCLK1:
    case LL_RCC_I3C2_CLKSOURCE_PCLK1:
      frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_I3C1_CLKSOURCE_CLKP:
    case LL_RCC_I3C2_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_I3C1_CLKSOURCE_IC10:
    case LL_RCC_I3C2_CLKSOURCE_IC10:
      if (LL_RCC_IC10_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC10_GetDivider();
        switch (LL_RCC_IC10_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_I3C1_CLKSOURCE_IC15:
    case LL_RCC_I3C2_CLKSOURCE_IC15:
      if (LL_RCC_IC15_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC15_GetDivider();
        switch (LL_RCC_IC15_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_I3C1_CLKSOURCE_HSI:
    case LL_RCC_I3C2_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_I3C1_CLKSOURCE_MSI:
    case LL_RCC_I3C2_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        frequency = MSI_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return LPTIMx clock frequency
  * @param  LPTIMxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE
  * @retval LPTIM clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetLPTIMClockFreq(uint32_t LPTIMxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_LPTIM_CLKSOURCE(LPTIMxSource));

  switch (LL_RCC_GetLPTIMClockSource(LPTIMxSource))
  {
    case LL_RCC_LPTIM1_CLKSOURCE_PCLK1:
      frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_LPTIM2_CLKSOURCE_PCLK4:
    case LL_RCC_LPTIM3_CLKSOURCE_PCLK4:
    case LL_RCC_LPTIM4_CLKSOURCE_PCLK4:
    case LL_RCC_LPTIM5_CLKSOURCE_PCLK4:
      frequency = RCC_GetPCLK4ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_LPTIM1_CLKSOURCE_CLKP:
    case LL_RCC_LPTIM2_CLKSOURCE_CLKP:
    case LL_RCC_LPTIM3_CLKSOURCE_CLKP:
    case LL_RCC_LPTIM4_CLKSOURCE_CLKP:
    case LL_RCC_LPTIM5_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_LPTIM1_CLKSOURCE_IC15:
    case LL_RCC_LPTIM2_CLKSOURCE_IC15:
    case LL_RCC_LPTIM3_CLKSOURCE_IC15:
    case LL_RCC_LPTIM4_CLKSOURCE_IC15:
    case LL_RCC_LPTIM5_CLKSOURCE_IC15:
      if (LL_RCC_IC15_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC15_GetDivider();
        switch (LL_RCC_IC15_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_LPTIM1_CLKSOURCE_LSE:
    case LL_RCC_LPTIM2_CLKSOURCE_LSE:
    case LL_RCC_LPTIM3_CLKSOURCE_LSE:
    case LL_RCC_LPTIM4_CLKSOURCE_LSE:
    case LL_RCC_LPTIM5_CLKSOURCE_LSE:
      if (LL_RCC_LSE_IsReady() != 0U)
      {
        frequency = LSE_VALUE;
      }
      break;

    case LL_RCC_LPTIM1_CLKSOURCE_LSI:
    case LL_RCC_LPTIM2_CLKSOURCE_LSI:
    case LL_RCC_LPTIM3_CLKSOURCE_LSI:
    case LL_RCC_LPTIM4_CLKSOURCE_LSI:
    case LL_RCC_LPTIM5_CLKSOURCE_LSI:
      if (LL_RCC_LSI_IsReady() != 0U)
      {
        frequency = LSI_VALUE;
      }
      break;

    case LL_RCC_LPTIM1_CLKSOURCE_TIMG:
    case LL_RCC_LPTIM2_CLKSOURCE_TIMG:
    case LL_RCC_LPTIM3_CLKSOURCE_TIMG:
    case LL_RCC_LPTIM4_CLKSOURCE_TIMG:
    case LL_RCC_LPTIM5_CLKSOURCE_TIMG:
      frequency = LL_RCC_GetSystemClockFreq() / (1UL << LL_RCC_GetTIMPrescaler());
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return LPUART clock frequency
  * @param  LPUARTxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE
  * @retval LPUART clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetLPUARTClockFreq(uint32_t LPUARTxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetLPUARTClockSource(LPUARTxSource))
  {
    case LL_RCC_LPUART1_CLKSOURCE_PCLK4:
      frequency = RCC_GetPCLK4ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_LPUART1_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_LPUART1_CLKSOURCE_IC9:
      if (LL_RCC_IC9_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC9_GetDivider();
        switch (LL_RCC_IC9_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_LPUART1_CLKSOURCE_IC14:
      if (LL_RCC_IC14_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC14_GetDivider();
        switch (LL_RCC_IC14_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_LPUART1_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_LPUART1_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_LPUART1_CLKSOURCE_LSE:
      if (LL_RCC_LSE_IsReady() != 0U)
      {
        frequency = LSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return LTDC clock frequency
  * @param  LTDCxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LTDC_CLKSOURCE
  * @retval LTDC clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetLTDCClockFreq(uint32_t LTDCxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_LTDC_CLKSOURCE(LTDCxSource));

  switch (LL_RCC_GetLTDCClockSource(LTDCxSource))
  {
    case LL_RCC_LTDC_CLKSOURCE_PCLK5:
      frequency = RCC_GetPCLK5ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_LTDC_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_LTDC_CLKSOURCE_IC16:
      if (LL_RCC_IC16_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC16_GetDivider();
        switch (LL_RCC_IC16_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_LTDC_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return MDFx clock frequency
  * @param  MDFxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE
  * @retval MDF clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetMDFClockFreq(uint32_t MDFxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_MDF_CLKSOURCE(MDFxSource));

  switch (LL_RCC_GetMDFClockSource(MDFxSource))
  {
    case LL_RCC_MDF1_CLKSOURCE_HCLK:
      frequency = RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq());
      break;

    case LL_RCC_MDF1_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_MDF1_CLKSOURCE_IC7:
      if (LL_RCC_IC7_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC7_GetDivider();
        switch (LL_RCC_IC7_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_MDF1_CLKSOURCE_IC8:
      if (LL_RCC_IC8_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC8_GetDivider();
        switch (LL_RCC_IC8_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_MDF1_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_MDF1_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_MDF1_CLKSOURCE_I2S_CKIN:
      frequency = EXTERNAL_CLOCK_VALUE;
      break;

    case LL_RCC_MDF1_CLKSOURCE_TIMG:
      frequency = LL_RCC_GetSystemClockFreq() / (1UL << LL_RCC_GetTIMPrescaler());
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return OTGPHYx clock frequency
  * @param  OTGPHYxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE
  * @retval OTGPHY clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetOTGPHYClockFreq(uint32_t OTGPHYxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_OTGPHY_CLKSOURCE(OTGPHYxSource));

  switch (LL_RCC_GetOTGPHYClockSource(OTGPHYxSource))
  {
    case LL_RCC_OTGPHY1_CLKSOURCE_CLKP:
    case LL_RCC_OTGPHY2_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_OTGPHY1_CLKSOURCE_IC15:
    case LL_RCC_OTGPHY2_CLKSOURCE_IC15:
      if (LL_RCC_IC15_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC15_GetDivider();
        switch (LL_RCC_IC15_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2:
    case LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        frequency = HSE_VALUE >> 1U;
      }
      break;

    case LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC:
    case LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2_OSC:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        if (LL_RCC_HSE_IsSelectedHSEDiv2AsDiv2Clock() == 0UL)
        {
          frequency = HSE_VALUE;
        }
        else
        {
          frequency = HSE_VALUE >> 1U;
        }
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return OTGPHYxCKREF clock frequency
  * @param  OTGPHYxCKREFSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE
  * @retval OTGPHYCKREF clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetOTGPHYCKREFClockFreq(uint32_t OTGPHYxCKREFSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_OTGPHYCKREF_CLKSOURCE(OTGPHYxCKREFSource));

  switch (LL_RCC_GetOTGPHYCKREFClockSource(OTGPHYxCKREFSource))
  {
    case LL_RCC_OTGPHY1CKREF_CLKSOURCE_OTGPHY1:
      frequency = LL_RCC_GetOTGPHYClockFreq(LL_RCC_OTGPHY1_CLKSOURCE);
      break;

    case LL_RCC_OTGPHY2CKREF_CLKSOURCE_OTGPHY2:
      frequency = LL_RCC_GetOTGPHYClockFreq(LL_RCC_OTGPHY2_CLKSOURCE);
      break;

    case LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC:
    case LL_RCC_OTGPHY2CKREF_CLKSOURCE_HSE_DIV_2_OSC:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        if (LL_RCC_HSE_IsSelectedHSEDiv2AsDiv2Clock() == 0UL)
        {
          frequency = HSE_VALUE;
        }
        else
        {
          frequency = HSE_VALUE >> 1U;
        }
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return PSSI clock frequency
  * @param  PSSIxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PSSI_CLKSOURCE
  * @retval PSSI clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetPSSIClockFreq(uint32_t PSSIxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_PSSI_CLKSOURCE(PSSIxSource));

  switch (LL_RCC_GetPSSIClockSource(PSSIxSource))
  {
    case LL_RCC_PSSI_CLKSOURCE_HCLK:
      frequency = RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq());
      break;

    case LL_RCC_PSSI_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_PSSI_CLKSOURCE_IC20:
      if (LL_RCC_IC20_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC20_GetDivider();
        switch (LL_RCC_IC20_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_PSSI_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return SAIx clock frequency
  * @param  SAIxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE
  * @retval SAI clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetSAIClockFreq(uint32_t SAIxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_SAI_CLKSOURCE(SAIxSource));

  switch (LL_RCC_GetSAIClockSource(SAIxSource))
  {
    case LL_RCC_SAI1_CLKSOURCE_PCLK2:
    case LL_RCC_SAI2_CLKSOURCE_PCLK2:
      frequency = RCC_GetPCLK2ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_SAI1_CLKSOURCE_CLKP:
    case LL_RCC_SAI2_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_SAI1_CLKSOURCE_IC7:
    case LL_RCC_SAI2_CLKSOURCE_IC7:
      if (LL_RCC_IC7_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC7_GetDivider();
        switch (LL_RCC_IC7_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_SAI1_CLKSOURCE_IC8:
    case LL_RCC_SAI2_CLKSOURCE_IC8:
      if (LL_RCC_IC8_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC8_GetDivider();
        switch (LL_RCC_IC8_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_SAI1_CLKSOURCE_HSI:
    case LL_RCC_SAI2_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_SAI1_CLKSOURCE_MSI:
    case LL_RCC_SAI2_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_SAI1_CLKSOURCE_I2S_CKIN:
    case LL_RCC_SAI2_CLKSOURCE_I2S_CKIN:
      frequency = EXTERNAL_CLOCK_VALUE;
      break;

    case LL_RCC_SAI1_CLKSOURCE_SPDIFRX1:
    case LL_RCC_SAI2_CLKSOURCE_SPDIFRX1:
      frequency = LL_RCC_GetSPDIFRXClockFreq(LL_RCC_SPDIFRX1_CLKSOURCE);
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return SDMMC clock frequency
  * @param  SDMMCxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE
  * @retval SDMMC clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetSDMMCClockFreq(uint32_t SDMMCxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_SDMMC_CLKSOURCE(SDMMCxSource));

  switch (LL_RCC_GetSDMMCClockSource(SDMMCxSource))
  {
    case LL_RCC_SDMMC1_CLKSOURCE_HCLK:
    case LL_RCC_SDMMC2_CLKSOURCE_HCLK:
      frequency = RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq());
      break;

    case LL_RCC_SDMMC1_CLKSOURCE_CLKP:
    case LL_RCC_SDMMC2_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_SDMMC1_CLKSOURCE_IC4:
    case LL_RCC_SDMMC2_CLKSOURCE_IC4:
      if (LL_RCC_IC4_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC4_GetDivider();
        switch (LL_RCC_IC4_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_SDMMC1_CLKSOURCE_IC5:
    case LL_RCC_SDMMC2_CLKSOURCE_IC5:
      if (LL_RCC_IC5_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC5_GetDivider();
        switch (LL_RCC_IC5_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return SPDIFRX clock frequency
  * @param  SPDIFRXxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE
  * @retval SPDIF clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetSPDIFRXClockFreq(uint32_t SPDIFRXxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_SPDIFRX_CLKSOURCE(SPDIFRXxSource));

  switch (LL_RCC_GetSPDIFRXClockSource(SPDIFRXxSource))
  {
    case LL_RCC_SPDIFRX1_CLKSOURCE_PCLK1:
      frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_SPDIFRX1_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_SPDIFRX1_CLKSOURCE_IC7:
      if (LL_RCC_IC7_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC7_GetDivider();
        switch (LL_RCC_IC7_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_SPDIFRX1_CLKSOURCE_IC8:
      if (LL_RCC_IC8_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC8_GetDivider();
        switch (LL_RCC_IC8_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_SPDIFRX1_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_SPDIFRX1_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_SPDIFRX1_CLKSOURCE_I2S_CKIN:
      frequency = EXTERNAL_CLOCK_VALUE;
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return SPIx clock frequency
  * @param  SPIxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE
  * @retval SPI clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetSPIClockFreq(uint32_t SPIxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_SPI_CLKSOURCE(SPIxSource));

  switch (LL_RCC_GetSPIClockSource(SPIxSource))
  {
    case LL_RCC_SPI2_CLKSOURCE_PCLK1:
    case LL_RCC_SPI3_CLKSOURCE_PCLK1:
      frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_SPI1_CLKSOURCE_PCLK2:
    case LL_RCC_SPI4_CLKSOURCE_PCLK2:
    case LL_RCC_SPI5_CLKSOURCE_PCLK2:
      frequency = RCC_GetPCLK2ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_SPI6_CLKSOURCE_PCLK4:
      frequency = RCC_GetPCLK4ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_SPI1_CLKSOURCE_CLKP:
    case LL_RCC_SPI2_CLKSOURCE_CLKP:
    case LL_RCC_SPI3_CLKSOURCE_CLKP:
    case LL_RCC_SPI4_CLKSOURCE_CLKP:
    case LL_RCC_SPI5_CLKSOURCE_CLKP:
    case LL_RCC_SPI6_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_SPI1_CLKSOURCE_IC8:
    case LL_RCC_SPI2_CLKSOURCE_IC8:
    case LL_RCC_SPI3_CLKSOURCE_IC8:
    case LL_RCC_SPI6_CLKSOURCE_IC8:
      if (LL_RCC_IC8_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC8_GetDivider();
        switch (LL_RCC_IC8_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_SPI1_CLKSOURCE_IC9:
    case LL_RCC_SPI2_CLKSOURCE_IC9:
    case LL_RCC_SPI3_CLKSOURCE_IC9:
    case LL_RCC_SPI4_CLKSOURCE_IC9:
    case LL_RCC_SPI5_CLKSOURCE_IC9:
    case LL_RCC_SPI6_CLKSOURCE_IC9:
      if (LL_RCC_IC9_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC9_GetDivider();
        switch (LL_RCC_IC9_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_SPI4_CLKSOURCE_IC14:
    case LL_RCC_SPI5_CLKSOURCE_IC14:
      if (LL_RCC_IC14_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC14_GetDivider();
        switch (LL_RCC_IC14_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_SPI1_CLKSOURCE_HSI:
    case LL_RCC_SPI2_CLKSOURCE_HSI:
    case LL_RCC_SPI3_CLKSOURCE_HSI:
    case LL_RCC_SPI4_CLKSOURCE_HSI:
    case LL_RCC_SPI5_CLKSOURCE_HSI:
    case LL_RCC_SPI6_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_SPI1_CLKSOURCE_MSI:
    case LL_RCC_SPI2_CLKSOURCE_MSI:
    case LL_RCC_SPI3_CLKSOURCE_MSI:
    case LL_RCC_SPI4_CLKSOURCE_MSI:
    case LL_RCC_SPI5_CLKSOURCE_MSI:
    case LL_RCC_SPI6_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_SPI1_CLKSOURCE_I2S_CKIN:
    case LL_RCC_SPI2_CLKSOURCE_I2S_CKIN:
    case LL_RCC_SPI3_CLKSOURCE_I2S_CKIN:
    case LL_RCC_SPI6_CLKSOURCE_I2S_CKIN:
      frequency = EXTERNAL_CLOCK_VALUE;
      break;

    case LL_RCC_SPI4_CLKSOURCE_HSE:
    case LL_RCC_SPI5_CLKSOURCE_HSE:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        frequency = HSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return UARTx clock frequency
  * @param  UARTxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_UART4_CLKSOURCE
  *         @arg @ref LL_RCC_UART5_CLKSOURCE
  *         @arg @ref LL_RCC_UART7_CLKSOURCE
  *         @arg @ref LL_RCC_UART8_CLKSOURCE
  *         @arg @ref LL_RCC_UART9_CLKSOURCE
  * @retval USART clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetUARTClockFreq(uint32_t UARTxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_UART_CLKSOURCE(UARTxSource));

  switch (LL_RCC_GetUARTClockSource(UARTxSource))
  {
    case LL_RCC_UART4_CLKSOURCE_PCLK1:
    case LL_RCC_UART5_CLKSOURCE_PCLK1:
    case LL_RCC_UART7_CLKSOURCE_PCLK1:
    case LL_RCC_UART8_CLKSOURCE_PCLK1:
      frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_UART9_CLKSOURCE_PCLK2:
      frequency = RCC_GetPCLK2ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_UART4_CLKSOURCE_CLKP:
    case LL_RCC_UART5_CLKSOURCE_CLKP:
    case LL_RCC_UART7_CLKSOURCE_CLKP:
    case LL_RCC_UART8_CLKSOURCE_CLKP:
    case LL_RCC_UART9_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_UART4_CLKSOURCE_IC9:
    case LL_RCC_UART5_CLKSOURCE_IC9:
    case LL_RCC_UART7_CLKSOURCE_IC9:
    case LL_RCC_UART8_CLKSOURCE_IC9:
    case LL_RCC_UART9_CLKSOURCE_IC9:
      if (LL_RCC_IC9_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC9_GetDivider();
        switch (LL_RCC_IC9_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_UART4_CLKSOURCE_IC14:
    case LL_RCC_UART5_CLKSOURCE_IC14:
    case LL_RCC_UART7_CLKSOURCE_IC14:
    case LL_RCC_UART8_CLKSOURCE_IC14:
    case LL_RCC_UART9_CLKSOURCE_IC14:
      if (LL_RCC_IC14_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC14_GetDivider();
        switch (LL_RCC_IC14_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_UART4_CLKSOURCE_HSI:
    case LL_RCC_UART5_CLKSOURCE_HSI:
    case LL_RCC_UART7_CLKSOURCE_HSI:
    case LL_RCC_UART8_CLKSOURCE_HSI:
    case LL_RCC_UART9_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_UART4_CLKSOURCE_MSI:
    case LL_RCC_UART5_CLKSOURCE_MSI:
    case LL_RCC_UART7_CLKSOURCE_MSI:
    case LL_RCC_UART8_CLKSOURCE_MSI:
    case LL_RCC_UART9_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_UART4_CLKSOURCE_LSE:
    case LL_RCC_UART5_CLKSOURCE_LSE:
    case LL_RCC_UART7_CLKSOURCE_LSE:
    case LL_RCC_UART8_CLKSOURCE_LSE:
    case LL_RCC_UART9_CLKSOURCE_LSE:
      if (LL_RCC_LSE_IsReady() != 0U)
      {
        frequency = LSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return USARTx clock frequency
  * @param  USARTxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_USART1_CLKSOURCE
  *         @arg @ref LL_RCC_USART2_CLKSOURCE
  *         @arg @ref LL_RCC_USART3_CLKSOURCE
  *         @arg @ref LL_RCC_USART6_CLKSOURCE
  *         @arg @ref LL_RCC_USART10_CLKSOURCE
  * @retval USART clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
uint32_t LL_RCC_GetUSARTClockFreq(uint32_t USARTxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  /* Check parameter */
  assert_param(IS_LL_RCC_USART_CLKSOURCE(USARTxSource));

  switch (LL_RCC_GetUSARTClockSource(USARTxSource))
  {
    case LL_RCC_USART1_CLKSOURCE_PCLK2:
    case LL_RCC_USART6_CLKSOURCE_PCLK2:
    case LL_RCC_USART10_CLKSOURCE_PCLK2:
      frequency = RCC_GetPCLK2ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_USART2_CLKSOURCE_PCLK1:
    case LL_RCC_USART3_CLKSOURCE_PCLK1:
      frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq()));
      break;

    case LL_RCC_USART1_CLKSOURCE_CLKP:
    case LL_RCC_USART2_CLKSOURCE_CLKP:
    case LL_RCC_USART3_CLKSOURCE_CLKP:
    case LL_RCC_USART6_CLKSOURCE_CLKP:
    case LL_RCC_USART10_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_USART1_CLKSOURCE_IC9:
    case LL_RCC_USART2_CLKSOURCE_IC9:
    case LL_RCC_USART3_CLKSOURCE_IC9:
    case LL_RCC_USART6_CLKSOURCE_IC9:
    case LL_RCC_USART10_CLKSOURCE_IC9:
      if (LL_RCC_IC9_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC9_GetDivider();
        switch (LL_RCC_IC9_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_USART1_CLKSOURCE_IC14:
    case LL_RCC_USART2_CLKSOURCE_IC14:
    case LL_RCC_USART3_CLKSOURCE_IC14:
    case LL_RCC_USART6_CLKSOURCE_IC14:
    case LL_RCC_USART10_CLKSOURCE_IC14:
      if (LL_RCC_IC14_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC14_GetDivider();
        switch (LL_RCC_IC14_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_USART1_CLKSOURCE_HSI:
    case LL_RCC_USART2_CLKSOURCE_HSI:
    case LL_RCC_USART3_CLKSOURCE_HSI:
    case LL_RCC_USART6_CLKSOURCE_HSI:
    case LL_RCC_USART10_CLKSOURCE_HSI:
      if (LL_RCC_HSI_IsReady() != 0U)
      {
        frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_USART1_CLKSOURCE_MSI:
    case LL_RCC_USART2_CLKSOURCE_MSI:
    case LL_RCC_USART3_CLKSOURCE_MSI:
    case LL_RCC_USART6_CLKSOURCE_MSI:
    case LL_RCC_USART10_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_USART1_CLKSOURCE_LSE:
    case LL_RCC_USART2_CLKSOURCE_LSE:
    case LL_RCC_USART3_CLKSOURCE_LSE:
    case LL_RCC_USART6_CLKSOURCE_LSE:
    case LL_RCC_USART10_CLKSOURCE_LSE:
      if (LL_RCC_LSE_IsReady() != 0U)
      {
        frequency = LSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return frequency;
}

/**
  * @brief  Return XSPI clock frequency
  * @param  XSPIxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE
  * @retval XSPI clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */

uint32_t LL_RCC_GetXSPIClockFreq(uint32_t XSPIxSource)
{
  uint32_t frequency = LL_RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetXSPIClockSource(XSPIxSource))
  {
    case LL_RCC_XSPI1_CLKSOURCE_HCLK:
    case LL_RCC_XSPI2_CLKSOURCE_HCLK:
    case LL_RCC_XSPI3_CLKSOURCE_HCLK:
      frequency = RCC_GetHCLKClockFreq(LL_RCC_GetSystemClockFreq());
      break;

    case LL_RCC_XSPI1_CLKSOURCE_CLKP:
    case LL_RCC_XSPI2_CLKSOURCE_CLKP:
    case LL_RCC_XSPI3_CLKSOURCE_CLKP:
      frequency = LL_RCC_GetCLKPClockFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_XSPI1_CLKSOURCE_IC3:
    case LL_RCC_XSPI2_CLKSOURCE_IC3:
    case LL_RCC_XSPI3_CLKSOURCE_IC3:
      if (LL_RCC_IC3_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC3_GetDivider();
        switch (LL_RCC_IC3_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    case LL_RCC_XSPI1_CLKSOURCE_IC4:
    case LL_RCC_XSPI2_CLKSOURCE_IC4:
    case LL_RCC_XSPI3_CLKSOURCE_IC4:
      if (LL_RCC_IC4_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC4_GetDivider();
        switch (LL_RCC_IC4_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            frequency = LL_RCC_GetPLL1ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            frequency = LL_RCC_GetPLL2ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            frequency = LL_RCC_GetPLL3ClockFreq();
            frequency = frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            frequency = LL_RCC_GetPLL4ClockFreq();
            frequency = frequency / ic_divider;
            break;
          default:
            /* Unexpected case */
            break;
        }
      }
      break;

    default:
      /* Nothing to do */
      break;
  }

  return frequency;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup RCC_LL_Private_Functions
  * @{
  */

/**
  * @brief  Return HCLK clock frequency
  * @param  SYSCLK_Frequency SYSCLK clock frequency
  * @retval HCLK clock frequency (in Hz)
  */
static uint32_t RCC_GetHCLKClockFreq(uint32_t SYSCLK_Frequency)
{
  /* HCLK clock frequency */
  return LL_RCC_CALC_HCLK_FREQ(SYSCLK_Frequency, LL_RCC_GetAHBPrescaler());
}

/**
  * @brief  Return PCLK1 clock frequency
  * @param  HCLK_Frequency HCLK clock frequency
  * @retval PCLK1 clock frequency (in Hz)
  */
static uint32_t RCC_GetPCLK1ClockFreq(uint32_t HCLK_Frequency)
{
  /* PCLK1 clock frequency */
  return LL_RCC_CALC_PCLK1_FREQ(HCLK_Frequency, LL_RCC_GetAPB1Prescaler());
}

/**
  * @brief  Return PCLK2 clock frequency
  * @param  HCLK_Frequency HCLK clock frequency
  * @retval PCLK2 clock frequency (in Hz)
  */
static uint32_t RCC_GetPCLK2ClockFreq(uint32_t HCLK_Frequency)
{
  /* PCLK2 clock frequency */
  return LL_RCC_CALC_PCLK2_FREQ(HCLK_Frequency, LL_RCC_GetAPB2Prescaler());
}

/**
  * @brief  Return PCLK4 clock frequency
  * @param  HCLK_Frequency HCLK clock frequency
  * @retval PCLK4 clock frequency (in Hz)
  */
static uint32_t RCC_GetPCLK4ClockFreq(uint32_t HCLK_Frequency)
{
  /* PCLK4 clock frequency */
  return LL_RCC_CALC_PCLK4_FREQ(HCLK_Frequency, LL_RCC_GetAPB4Prescaler());
}

/**
  * @brief  Return PCLK5 clock frequency
  * @param  HCLK_Frequency HCLK clock frequency
  * @retval PCLK5 clock frequency (in Hz)
  */
static uint32_t RCC_GetPCLK5ClockFreq(uint32_t HCLK_Frequency)
{
  /* PCLK5 clock frequency */
  return LL_RCC_CALC_PCLK5_FREQ(HCLK_Frequency, LL_RCC_GetAPB5Prescaler());
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* defined(RCC) */

/**
  * @}
  */
