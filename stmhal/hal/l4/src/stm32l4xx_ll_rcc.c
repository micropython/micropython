/**
  ******************************************************************************
  * @file    stm32l4xx_ll_rcc.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   RCC LL module driver.
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
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_ll_rcc.h"
#ifdef  USE_FULL_ASSERT
  #include "stm32_assert.h"
#else
  #define assert_param(expr) ((void)0)
#endif
/** @addtogroup STM32L4xx_LL_Driver
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
#define IS_LL_RCC_USART_CLKSOURCE(__VALUE__)  (((__VALUE__) == LL_RCC_USART1_CLKSOURCE) \
                                            || ((__VALUE__) == LL_RCC_USART2_CLKSOURCE) \
                                            || ((__VALUE__) == LL_RCC_USART3_CLKSOURCE))

#if defined(UART4) && defined(UART5)
#define IS_LL_RCC_UART_CLKSOURCE(__VALUE__)    (((__VALUE__) == LL_RCC_UART4_CLKSOURCE) \
                                             || ((__VALUE__) == LL_RCC_UART5_CLKSOURCE))
#elif defined(UART4)
#define IS_LL_RCC_UART_INSTANCE(__VALUE__)     ((__VALUE__) == LL_RCC_UART4_CLKSOURCE)
#elif defined(UART5)
#define IS_LL_RCC_UART_INSTANCE(__VALUE__)     ((__VALUE__) == LL_RCC_UART5_CLKSOURCE)
#endif /* UART4 && UART5*/

#define IS_LL_RCC_LPUART_CLKSOURCE(__VALUE__) (((__VALUE__) == LL_RCC_LPUART1_CLKSOURCE))

#define IS_LL_RCC_I2C_CLKSOURCE(__VALUE__)    (((__VALUE__) == LL_RCC_I2C1_CLKSOURCE) \
                                            || ((__VALUE__) == LL_RCC_I2C2_CLKSOURCE) \
                                            || ((__VALUE__) == LL_RCC_I2C3_CLKSOURCE))

#define IS_LL_RCC_LPTIM_CLKSOURCE(__VALUE__)  (((__VALUE__) == LL_RCC_LPTIM1_CLKSOURCE) \
                                            || ((__VALUE__) == LL_RCC_LPTIM2_CLKSOURCE))

#if defined(SAI2)
#define IS_LL_RCC_SAI_CLKSOURCE(__VALUE__)    (((__VALUE__) == LL_RCC_SAI1_CLKSOURCE) \
                                            || ((__VALUE__) == LL_RCC_SAI2_CLKSOURCE))
#else
#define IS_LL_RCC_SAI_CLKSOURCE(__VALUE__)    ((__VALUE__) == LL_RCC_SAI1_CLKSOURCE)
#endif /*SAI2*/

#define IS_LL_RCC_SDMMC_CLKSOURCE(__VALUE__)  (((__VALUE__) == LL_RCC_SDMMC1_CLKSOURCE))

#define IS_LL_RCC_RNG_CLKSOURCE(__VALUE__)    (((__VALUE__) == LL_RCC_RNG_CLKSOURCE))

#if defined(USB_OTG_FS)
#define IS_LL_RCC_USB_CLKSOURCE(__VALUE__)    (((__VALUE__) == LL_RCC_USB_CLKSOURCE))
#endif /* USB_OTG_FS */

#define IS_LL_RCC_ADC_CLKSOURCE(__VALUE__)    (((__VALUE__) == LL_RCC_ADC_CLKSOURCE))

#define IS_LL_RCC_SWPMI_CLKSOURCE(__VALUE__)  (((__VALUE__) == LL_RCC_SWPMI1_CLKSOURCE))

#define IS_LL_RCC_DFSDM_CLKSOURCE(__VALUE__)  (((__VALUE__) == LL_RCC_DFSDM_CLKSOURCE))

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup RCC_LL_Private_Functions RCC Private functions
  * @{
  */
uint32_t RCC_GetSystemClockFreq(void);
uint32_t RCC_GetHCLKClockFreq(uint32_t SYSCLK_Frequency);
uint32_t RCC_GetPCLK1ClockFreq(uint32_t HCLK_Frequency);
uint32_t RCC_GetPCLK2ClockFreq(uint32_t HCLK_Frequency);
uint32_t RCC_PLL_GetFreqDomain_SYS(void);
uint32_t RCC_PLL_GetFreqDomain_SAI(void);
uint32_t RCC_PLL_GetFreqDomain_48M(void);
uint32_t RCC_PLLSAI1_GetFreqDomain_SAI(void);
uint32_t RCC_PLLSAI1_GetFreqDomain_48M(void);
uint32_t RCC_PLLSAI1_GetFreqDomain_ADC(void);
#if defined(RCC_PLLSAI2_SUPPORT)
uint32_t RCC_PLLSAI2_GetFreqDomain_SAI(void);
uint32_t RCC_PLLSAI2_GetFreqDomain_ADC(void);
#endif /*RCC_PLLSAI2_SUPPORT*/
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
  * @brief  Reset the RCC clock configuration to the default reset state.
  * @note   The default reset state of the clock configuration is given below:
  *         - MSI  ON and used as system clock source
  *         - HSE, HSI, PLL and PLLSAIxSource OFF
  *         - AHB, APB1 and APB2 prescaler set to 1.
  *         - CSS, MCO OFF
  *         - All interrupts disabled
  * @note   This function doesn't modify the configuration of the
  *         - Peripheral clocks
  *         - LSI, LSE and RTC clocks
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: RCC registers are de-initialized
  *          - ERROR: not applicable
  */
ErrorStatus LL_RCC_DeInit(void)
{
  uint32_t vl_mask = 0;

  /* Set MSION bit */
  LL_RCC_MSI_Enable();

  /* Insure MSIRDY bit is set before writing default MSIRANGE value */
  while (LL_RCC_MSI_IsReady() == 0)
  {
    __NOP();
  }

  /* Set MSIRANGE default value */
  LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);
  /* Set MSITRIM bits to the reset value*/
  LL_RCC_MSI_SetCalibTrimming(0);

  /* Set HSITRIM bits to the reset value*/
  LL_RCC_HSI_SetCalibTrimming(0x10);

  /* Reset CFGR register */
  LL_RCC_WriteReg(CFGR, 0x00000000);

  vl_mask = 0xFFFFFFFFU;

  /* Reset HSION, HSIKERON, HSIASFS, HSEON, PLLSYSON bits */
  CLEAR_BIT(vl_mask, (RCC_CR_HSION | RCC_CR_HSIASFS | RCC_CR_HSIKERON  | RCC_CR_HSEON |
  RCC_CR_PLLON));

  /* Reset PLLSAI1ON bit */
  CLEAR_BIT(vl_mask, RCC_CR_PLLSAI1ON);

#if defined(RCC_PLLSAI2_SUPPORT)
  /* Reset PLLSAI2ON bit */
  CLEAR_BIT(vl_mask, RCC_CR_PLLSAI2ON);
#endif /*RCC_PLLSAI2_SUPPORT*/

  /* Write new mask in CR register */
  LL_RCC_WriteReg(CR, vl_mask);

  /* Reset PLLCFGR register */
  LL_RCC_WriteReg(PLLCFGR, 16 << RCC_POSITION_PLLN);

  /* Reset PLLSAI1CFGR register */
  LL_RCC_WriteReg(PLLSAI1CFGR, 16 << RCC_POSITION_PLLSAI1N);

#if defined(RCC_PLLSAI2_SUPPORT)
  /* Reset PLLSAI2CFGR register */
  LL_RCC_WriteReg(PLLSAI2CFGR, 16 << RCC_POSITION_PLLSAI2N);
#endif /*RCC_PLLSAI2_SUPPORT*/

  /* Reset HSEBYP bit */
  LL_RCC_HSE_DisableBypass();

  /* Disable all interrupts */
  LL_RCC_WriteReg(CIER, 0x00000000);

  return SUCCESS;
}

/**
  * @}
  */

/** @addtogroup RCC_LL_EF_Get_Freq
  * @brief  Return the frequencies of different on chip clocks;  System, AHB, APB1 and APB2 buses clocks
  *         and different peripheral clocks available on the device.
  * @note   If SYSCLK source is MSI, function returns values based on MSI_VALUE(*)
  * @note   If SYSCLK source is HSI, function returns values based on HSI_VALUE(**)
  * @note   If SYSCLK source is HSE, function returns values based on HSE_VALUE(***)
  * @note   If SYSCLK source is PLL, function returns values based on HSE_VALUE(***)
  *         or HSI_VALUE(**) or MSI_VALUE(*) multiplied/divided by the PLL factors.
  * @note   (*) MSI_VALUE is a constant defined in this file (default value
  *             4 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.
  * @note   (**) HSI_VALUE is a constant defined in this file (default value
  *              16 MHz) but the real value may vary depending on the variations
  *              in voltage and temperature.
  * @note   (***) HSE_VALUE is a constant defined in this file (default value
  *               8 MHz), user has to ensure that HSE_VALUE is same as the real
  *               frequency of the crystal used. Otherwise, this function may
  *               have wrong result.
  * @note   The result of this function could be incorrect when using fractional
  *         value for HSE crystal.
  * @note   This function can be used by the user application to compute the
  *         baud-rate for the communication peripherals or configure other parameters.
  * @{
  */

/**
  * @brief  Return the frequencies of different on chip clocks;  System, AHB, APB1 and APB2 buses clocks
  * @note   Each time SYSCLK, HCLK, PCLK1 and/or PCLK2 clock changes, this function
  *         must be called to update structure fields. Otherwise, any
  *         configuration based on this function will be incorrect.
  * @param  RCC_Clocks pointer to a @ref LL_RCC_ClocksTypeDef structure which will hold the clocks frequencies
  * @retval None
  */
void LL_RCC_GetSystemClocksFreq(LL_RCC_ClocksTypeDef *RCC_Clocks)
{
  /* Get SYSCLK frequency */
  RCC_Clocks->SYSCLK_Frequency = RCC_GetSystemClockFreq();

  /* HCLK clock frequency */
  RCC_Clocks->HCLK_Frequency   = RCC_GetHCLKClockFreq(RCC_Clocks->SYSCLK_Frequency);

  /* PCLK1 clock frequency */
  RCC_Clocks->PCLK1_Frequency  = RCC_GetPCLK1ClockFreq(RCC_Clocks->HCLK_Frequency);

  /* PCLK2 clock frequency */
  RCC_Clocks->PCLK2_Frequency  = RCC_GetPCLK2ClockFreq(RCC_Clocks->HCLK_Frequency);
}

/**
  * @brief  Return USARTx clock frequency
  * @param  USARTxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_USART1_CLKSOURCE
  *         @arg @ref LL_RCC_USART2_CLKSOURCE
  *         @arg @ref LL_RCC_USART3_CLKSOURCE
  * @retval USART clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator (HSI or LSE) is not ready
  */
uint32_t LL_RCC_GetUSARTClockFreq(uint32_t USARTxSource)
{
  uint32_t usart_frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_USART_CLKSOURCE(USARTxSource));

  if (USARTxSource == LL_RCC_USART1_CLKSOURCE)
  {
    /* USART1CLK clock frequency */
    switch (LL_RCC_GetUSARTClockSource(USARTxSource))
    {
      case LL_RCC_USART1_CLKSOURCE_SYSCLK: /* USART1 Clock is System Clock */
        usart_frequency = RCC_GetSystemClockFreq();
        break;

      case LL_RCC_USART1_CLKSOURCE_HSI:    /* USART1 Clock is HSI Osc. */
        if (LL_RCC_HSI_IsReady())
        {
          usart_frequency = HSI_VALUE;
        }
        break;

      case LL_RCC_USART1_CLKSOURCE_LSE:    /* USART1 Clock is LSE Osc. */
        if (LL_RCC_LSE_IsReady())
        {
          usart_frequency = LSE_VALUE;
        }
        break;

      case LL_RCC_USART1_CLKSOURCE_PCLK2:  /* USART1 Clock is PCLK2 */
      default:
        usart_frequency = RCC_GetPCLK2ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
        break;
    }
  }
  else if (USARTxSource == LL_RCC_USART2_CLKSOURCE)
  {
    /* USART2CLK clock frequency */
    switch (LL_RCC_GetUSARTClockSource(USARTxSource))
    {
      case LL_RCC_USART2_CLKSOURCE_SYSCLK: /* USART2 Clock is System Clock */
        usart_frequency = RCC_GetSystemClockFreq();
        break;

      case LL_RCC_USART2_CLKSOURCE_HSI:    /* USART2 Clock is HSI Osc. */
        if (LL_RCC_HSI_IsReady())
        {
          usart_frequency = HSI_VALUE;
        }
        break;

      case LL_RCC_USART2_CLKSOURCE_LSE:    /* USART2 Clock is LSE Osc. */
        if (LL_RCC_LSE_IsReady())
        {
          usart_frequency = LSE_VALUE;
        }
        break;

      case LL_RCC_USART2_CLKSOURCE_PCLK1:  /* USART2 Clock is PCLK1 */
      default:
        usart_frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
        break;
    }
  }
  else
  {
    if (USARTxSource == LL_RCC_USART3_CLKSOURCE)
    {
      /* USART3CLK clock frequency */
      switch (LL_RCC_GetUSARTClockSource(USARTxSource))
      {
        case LL_RCC_USART3_CLKSOURCE_SYSCLK: /* USART3 Clock is System Clock */
          usart_frequency = RCC_GetSystemClockFreq();
          break;

        case LL_RCC_USART3_CLKSOURCE_HSI:    /* USART3 Clock is HSI Osc. */
          if (LL_RCC_HSI_IsReady())
          {
            usart_frequency = HSI_VALUE;
          }
          break;

        case LL_RCC_USART3_CLKSOURCE_LSE:    /* USART3 Clock is LSE Osc. */
          if (LL_RCC_LSE_IsReady())
          {
            usart_frequency = LSE_VALUE;
          }
          break;

        case LL_RCC_USART3_CLKSOURCE_PCLK1:  /* USART3 Clock is PCLK1 */
        default:
          usart_frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
          break;
      }
    }
  }
  return usart_frequency;
}

#if defined(UART4) || defined(UART5)
/**
  * @brief  Return UARTx clock frequency
  * @param  UARTxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_UART4_CLKSOURCE
  *         @arg @ref LL_RCC_UART5_CLKSOURCE
  * @retval UART clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator (HSI or LSE) is not ready
  */
uint32_t LL_RCC_GetUARTClockFreq(uint32_t UARTxSource)
{
  uint32_t uart_frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_UART_CLKSOURCE(UARTxSource));

#if defined(UART4)
  if (UARTxSource == LL_RCC_UART4_CLKSOURCE)
  {
    /* UART4CLK clock frequency */
    switch (LL_RCC_GetUARTClockSource(UARTxSource))
    {
      case LL_RCC_UART4_CLKSOURCE_SYSCLK: /* UART4 Clock is System Clock */
        uart_frequency = RCC_GetSystemClockFreq();
        break;

      case LL_RCC_UART4_CLKSOURCE_HSI:    /* UART4 Clock is HSI Osc. */
        if (LL_RCC_HSI_IsReady())
        {
          uart_frequency = HSI_VALUE;
        }
        break;

      case LL_RCC_UART4_CLKSOURCE_LSE:    /* UART4 Clock is LSE Osc. */
        if (LL_RCC_LSE_IsReady())
        {
          uart_frequency = LSE_VALUE;
        }
        break;

      case LL_RCC_UART4_CLKSOURCE_PCLK1:  /* UART4 Clock is PCLK1 */
      default:
        uart_frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
        break;
    }
  }
#endif /* UART4 */

#if defined(UART5)
  if (UARTxSource == LL_RCC_UART5_CLKSOURCE)
  {
    /* UART5CLK clock frequency */
    switch (LL_RCC_GetUARTClockSource(UARTxSource))
    {
      case LL_RCC_UART5_CLKSOURCE_SYSCLK: /* UART5 Clock is System Clock */
        uart_frequency = RCC_GetSystemClockFreq();
        break;

      case LL_RCC_UART5_CLKSOURCE_HSI:    /* UART5 Clock is HSI Osc. */
        if (LL_RCC_HSI_IsReady())
        {
          uart_frequency = HSI_VALUE;
        }
        break;

      case LL_RCC_UART5_CLKSOURCE_LSE:    /* UART5 Clock is LSE Osc. */
        if (LL_RCC_LSE_IsReady())
        {
          uart_frequency = LSE_VALUE;
        }
        break;

      case LL_RCC_UART5_CLKSOURCE_PCLK1:  /* UART5 Clock is PCLK1 */
      default:
        uart_frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
        break;
    }
  }
#endif /* UART5 */

  return uart_frequency;
}
#endif /* UART4 || UART5 */

/**
  * @brief  Return I2Cx clock frequency
  * @param  I2CxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE
  * @retval I2C clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that HSI oscillator is not ready
  */
uint32_t LL_RCC_GetI2CClockFreq(uint32_t I2CxSource)
{
  uint32_t i2c_frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_I2C_CLKSOURCE(I2CxSource));

  if (I2CxSource == LL_RCC_I2C1_CLKSOURCE)
  {
    /* I2C1 CLK clock frequency */
    switch (LL_RCC_GetI2CClockSource(I2CxSource))
    {
      case LL_RCC_I2C1_CLKSOURCE_SYSCLK: /* I2C1 Clock is System Clock */
        i2c_frequency = RCC_GetSystemClockFreq();
        break;

      case LL_RCC_I2C1_CLKSOURCE_HSI:    /* I2C1 Clock is HSI Osc. */
        if (LL_RCC_HSI_IsReady())
        {
          i2c_frequency = HSI_VALUE;
        }
        break;

      case LL_RCC_I2C1_CLKSOURCE_PCLK1:  /* I2C1 Clock is PCLK1 */
      default:
        i2c_frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
        break;
    }
  }
  else if (I2CxSource == LL_RCC_I2C2_CLKSOURCE)
  {
    /* I2C2 CLK clock frequency */
    switch (LL_RCC_GetI2CClockSource(I2CxSource))
    {
      case LL_RCC_I2C2_CLKSOURCE_SYSCLK: /* I2C2 Clock is System Clock */
        i2c_frequency = RCC_GetSystemClockFreq();
        break;

      case LL_RCC_I2C2_CLKSOURCE_HSI:    /* I2C2 Clock is HSI Osc. */
        if (LL_RCC_HSI_IsReady())
        {
          i2c_frequency = HSI_VALUE;
        }
        break;

      case LL_RCC_I2C2_CLKSOURCE_PCLK1:  /* I2C2 Clock is PCLK1 */
      default:
        i2c_frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
        break;
    }
  }
  else
  {
    if (I2CxSource == LL_RCC_I2C3_CLKSOURCE)
    {
      /* I2C3 CLK clock frequency */
      switch (LL_RCC_GetI2CClockSource(I2CxSource))
      {
        case LL_RCC_I2C3_CLKSOURCE_SYSCLK: /* I2C3 Clock is System Clock */
          i2c_frequency = RCC_GetSystemClockFreq();
          break;

        case LL_RCC_I2C3_CLKSOURCE_HSI:    /* I2C3 Clock is HSI Osc. */
          if (LL_RCC_HSI_IsReady())
          {
            i2c_frequency = HSI_VALUE;
          }
          break;

        case LL_RCC_I2C3_CLKSOURCE_PCLK1:  /* I2C3 Clock is PCLK1 */
        default:
          i2c_frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
          break;
      }
    }
  }

  return i2c_frequency;
}

/**
  * @brief  Return LPUARTx clock frequency
  * @param  LPUARTxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE
  * @retval LPUART clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator (HSI or LSE) is not ready
  */
uint32_t LL_RCC_GetLPUARTClockFreq(uint32_t LPUARTxSource)
{
  uint32_t lpuart_frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_LPUART_CLKSOURCE(LPUARTxSource));

  /* LPUART1CLK clock frequency */
  switch (LL_RCC_GetLPUARTClockSource(LPUARTxSource))
  {
    case LL_RCC_LPUART1_CLKSOURCE_SYSCLK: /* LPUART1 Clock is System Clock */
      lpuart_frequency = RCC_GetSystemClockFreq();
      break;

    case LL_RCC_LPUART1_CLKSOURCE_HSI:    /* LPUART1 Clock is HSI Osc. */
      if (LL_RCC_HSI_IsReady())
      {
        lpuart_frequency = HSI_VALUE;
      }
      break;

    case LL_RCC_LPUART1_CLKSOURCE_LSE:    /* LPUART1 Clock is LSE Osc. */
      if (LL_RCC_LSE_IsReady())
      {
        lpuart_frequency = LSE_VALUE;
      }
      break;

    case LL_RCC_LPUART1_CLKSOURCE_PCLK1:  /* LPUART1 Clock is PCLK1 */
    default:
      lpuart_frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
      break;
  }

  return lpuart_frequency;
}

/**
  * @brief  Return LPTIMx clock frequency
  * @param  LPTIMxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE
  * @retval LPTIM clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator (HSI or LSE) is not ready
  */
uint32_t LL_RCC_GetLPTIMClockFreq(uint32_t LPTIMxSource)
{
  uint32_t lptim_frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_LPTIM_CLKSOURCE(LPTIMxSource));

  if (LPTIMxSource == LL_RCC_LPTIM1_CLKSOURCE)
  {
    /* LPTIM1CLK clock frequency */
    switch (LL_RCC_GetLPTIMClockSource(LPTIMxSource))
    {
      case LL_RCC_LPTIM1_CLKSOURCE_LSI:    /* LPTIM1 Clock is LSI Osc. */
        if (LL_RCC_LSI_IsReady())
        {
          lptim_frequency = LSI_VALUE;
        }
        break;

      case LL_RCC_LPTIM1_CLKSOURCE_HSI:    /* LPTIM1 Clock is HSI Osc. */
        if (LL_RCC_HSI_IsReady())
        {
          lptim_frequency = HSI_VALUE;
        }
        break;

      case LL_RCC_LPTIM1_CLKSOURCE_LSE:    /* LPTIM1 Clock is LSE Osc. */
        if (LL_RCC_LSE_IsReady())
        {
          lptim_frequency = LSE_VALUE;
        }
        break;

      case LL_RCC_LPTIM1_CLKSOURCE_PCLK1:  /* LPTIM1 Clock is PCLK1 */
      default:
        lptim_frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
        break;
    }
  }
  else
  {
    if (LPTIMxSource == LL_RCC_LPTIM2_CLKSOURCE)
    {
      /* LPTIM2CLK clock frequency */
      switch (LL_RCC_GetLPTIMClockSource(LPTIMxSource))
      {
        case LL_RCC_LPTIM2_CLKSOURCE_LSI:    /* LPTIM2 Clock is LSI Osc. */
          if (LL_RCC_LSI_IsReady())
          {
            lptim_frequency = LSI_VALUE;
          }
          break;

        case LL_RCC_LPTIM2_CLKSOURCE_HSI:    /* LPTIM2 Clock is HSI Osc. */
          if (LL_RCC_HSI_IsReady())
          {
            lptim_frequency = HSI_VALUE;
          }
          break;

        case LL_RCC_LPTIM2_CLKSOURCE_LSE:    /* LPTIM2 Clock is LSE Osc. */
          if (LL_RCC_LSE_IsReady())
          {
            lptim_frequency = LSE_VALUE;
          }
          break;

        case LL_RCC_LPTIM2_CLKSOURCE_PCLK1:  /* LPTIM2 Clock is PCLK1 */
        default:
          lptim_frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
          break;
      }
    }      
  }

  return lptim_frequency;
}

/**
  * @brief  Return SAIx clock frequency
  * @param  SAIxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE (*)
  *
  *         (*) value not defined in all devices.
  * @retval SAI clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that PLL is not ready
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NA indicates that external clock is used
  */
uint32_t LL_RCC_GetSAIClockFreq(uint32_t SAIxSource)
{
  uint32_t sai_frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_SAI_CLKSOURCE(SAIxSource));

  if (SAIxSource == LL_RCC_SAI1_CLKSOURCE)
  {
    /* SAI1CLK clock frequency */
    switch (LL_RCC_GetSAIClockSource(SAIxSource))
    {
      case LL_RCC_SAI1_CLKSOURCE_PLLSAI1:    /* PLLSAI1 clock used as SAI1 clock source */
        if (LL_RCC_PLLSAI1_IsReady())
        {
          sai_frequency = RCC_PLLSAI1_GetFreqDomain_SAI();
        }
        break;

#if defined(RCC_PLLSAI2_SUPPORT)
      case LL_RCC_SAI1_CLKSOURCE_PLLSAI2:    /* PLLSAI2 clock used as SAI1 clock source */
        if (LL_RCC_PLLSAI2_IsReady())
        {
          sai_frequency = RCC_PLLSAI2_GetFreqDomain_SAI();
        }
        break;

#endif /* RCC_PLLSAI2_SUPPORT */
      case LL_RCC_SAI1_CLKSOURCE_PLL:        /* PLL clock used as SAI1 clock source */
        if (LL_RCC_PLL_IsReady())
        {
          sai_frequency = RCC_PLL_GetFreqDomain_SAI();
        }
        break;

      case LL_RCC_SAI1_CLKSOURCE_PIN:        /* External input clock used as SAI1 clock source */
      default:
        sai_frequency = LL_RCC_PERIPH_FREQUENCY_NA;
        break;
    }
  }
  else
  {
#if defined(SAI2)
    if (SAIxSource == LL_RCC_SAI2_CLKSOURCE)
    {
      /* SAI2CLK clock frequency */
      switch (LL_RCC_GetSAIClockSource(SAIxSource))
      {
        case LL_RCC_SAI2_CLKSOURCE_PLLSAI1:  /* PLLSAI1 clock used as SAI2 clock source */
          if (LL_RCC_PLLSAI1_IsReady())
          {
            sai_frequency = RCC_PLLSAI1_GetFreqDomain_SAI();
          }
          break;

  #if defined(RCC_PLLSAI2_SUPPORT)
        case LL_RCC_SAI2_CLKSOURCE_PLLSAI2:  /* PLLSAI2 clock used as SAI2 clock source */
          if (LL_RCC_PLLSAI2_IsReady())
          {
            sai_frequency = RCC_PLLSAI2_GetFreqDomain_SAI();
          }
          break;

  #endif /* RCC_PLLSAI2_SUPPORT */
        case LL_RCC_SAI2_CLKSOURCE_PLL:      /* PLL clock used as SAI2 clock source */
          if (LL_RCC_PLL_IsReady())
          {
            sai_frequency = RCC_PLL_GetFreqDomain_SAI();
          }
          break;

        case LL_RCC_SAI2_CLKSOURCE_PIN:      /* External input clock used as SAI2 clock source */
        default:
          sai_frequency = LL_RCC_PERIPH_FREQUENCY_NA;
          break;
      }
    }
#endif /*SAI2*/
  }

  return sai_frequency;
}

/**
  * @brief  Return SDMMCx clock frequency
  * @param  SDMMCxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE
  * @retval SDMMC clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator (MSI) or PLL is not ready
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NA indicates that no clock source selected
  */
uint32_t LL_RCC_GetSDMMCClockFreq(uint32_t SDMMCxSource)
{
  uint32_t sdmmc_frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_SDMMC_CLKSOURCE(SDMMCxSource));

  /* SDMMC1CLK clock frequency */
  switch (LL_RCC_GetSDMMCClockSource(SDMMCxSource))
  {
    case LL_RCC_SDMMC1_CLKSOURCE_PLLSAI1:    /* PLLSAI1 clock used as SDMMC1 clock source */
      if (LL_RCC_PLLSAI1_IsReady())
      {
        sdmmc_frequency = RCC_PLLSAI1_GetFreqDomain_48M();
      }
      break;

    case LL_RCC_SDMMC1_CLKSOURCE_PLL:        /* PLL clock used as SDMMC1 clock source */
      if (LL_RCC_PLL_IsReady())
      {
        sdmmc_frequency = RCC_PLL_GetFreqDomain_48M();
      }
      break;

    case LL_RCC_SDMMC1_CLKSOURCE_MSI:        /* MSI clock used as SDMMC1 clock source */
      if (LL_RCC_MSI_IsReady())
      {
        sdmmc_frequency = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      }
      break;

    case LL_RCC_SDMMC1_CLKSOURCE_NONE:       /* No clock used as SDMMC1 clock source */
    default:
      sdmmc_frequency = LL_RCC_PERIPH_FREQUENCY_NA;
      break;
  }

  return sdmmc_frequency;
}

/**
  * @brief  Return RNGx clock frequency
  * @param  RNGxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_RNG_CLKSOURCE
  * @retval RNG clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator (MSI) or PLL is not ready
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NA indicates that no clock source selected
  */
uint32_t LL_RCC_GetRNGClockFreq(uint32_t RNGxSource)
{
  uint32_t rng_frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_RNG_CLKSOURCE(RNGxSource));

  /* RNGCLK clock frequency */
  switch (LL_RCC_GetRNGClockSource(RNGxSource))
  {
    case LL_RCC_RNG_CLKSOURCE_PLLSAI1:       /* PLLSAI1 clock used as RNG clock source */
      if (LL_RCC_PLLSAI1_IsReady())
      {
        rng_frequency = RCC_PLLSAI1_GetFreqDomain_48M();
      }
      break;

    case LL_RCC_RNG_CLKSOURCE_PLL:           /* PLL clock used as RNG clock source */
      if (LL_RCC_PLL_IsReady())
      {
        rng_frequency = RCC_PLL_GetFreqDomain_48M();
      }
      break;

    case LL_RCC_RNG_CLKSOURCE_MSI:           /* MSI clock used as RNG clock source */
      if (LL_RCC_MSI_IsReady())
      {
        rng_frequency = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      }
      break;

    case LL_RCC_RNG_CLKSOURCE_NONE:          /* No clock used as RNG clock source */
    default:
      rng_frequency = LL_RCC_PERIPH_FREQUENCY_NA;
      break;
  }

  return rng_frequency;
}

#if defined(USB_OTG_FS)
/**
  * @brief  Return USBx clock frequency
  * @param  USBxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_USB_CLKSOURCE
  * @retval USB clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator (MSI) or PLL is not ready
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NA indicates that no clock source selected
  */
uint32_t LL_RCC_GetUSBClockFreq(uint32_t USBxSource)
{
  uint32_t usb_frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_USB_CLKSOURCE(USBxSource));

  /* USBCLK clock frequency */
  switch (LL_RCC_GetUSBClockSource(USBxSource))
  {
    case LL_RCC_USB_CLKSOURCE_PLLSAI1:       /* PLLSAI1 clock used as USB clock source */
      if (LL_RCC_PLLSAI1_IsReady())
      {
        usb_frequency = RCC_PLLSAI1_GetFreqDomain_48M();
      }
      break;

    case LL_RCC_USB_CLKSOURCE_PLL:           /* PLL clock used as USB clock source */
      if (LL_RCC_PLL_IsReady())
      {
        usb_frequency = RCC_PLL_GetFreqDomain_48M();
      }
      break;

    case LL_RCC_USB_CLKSOURCE_MSI:           /* MSI clock used as USB clock source */
      if (LL_RCC_MSI_IsReady())
      {
        usb_frequency = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      }
      break;

    case LL_RCC_USB_CLKSOURCE_NONE:          /* No clock used as USB clock source */
    default:
      usb_frequency = LL_RCC_PERIPH_FREQUENCY_NA;
      break;
  }

  return usb_frequency;
}
#endif /* USB_OTG_FS */

/**
  * @brief  Return ADCx clock frequency
  * @param  ADCxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ADC_CLKSOURCE
  * @retval ADC clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator (MSI) or PLL is not ready
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NA indicates that no clock source selected
  */
uint32_t LL_RCC_GetADCClockFreq(uint32_t ADCxSource)
{
  uint32_t adc_frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_ADC_CLKSOURCE(ADCxSource));

  /* ADCCLK clock frequency */
  switch (LL_RCC_GetADCClockSource(ADCxSource))
  {
    case LL_RCC_ADC_CLKSOURCE_PLLSAI1:       /* PLLSAI1 clock used as ADC clock source */
      if (LL_RCC_PLLSAI1_IsReady())
      {
        adc_frequency = RCC_PLLSAI1_GetFreqDomain_ADC();
      }
      break;

#if defined(RCC_PLLSAI2_SUPPORT)
    case LL_RCC_ADC_CLKSOURCE_PLLSAI2:       /* PLLSAI2 clock used as ADC clock source */
      if (LL_RCC_PLLSAI2_IsReady())
      {
         adc_frequency = RCC_PLLSAI2_GetFreqDomain_ADC();
      }
      break;
#endif /* RCC_PLLSAI2_SUPPORT */

    case LL_RCC_ADC_CLKSOURCE_SYSCLK:        /* SYSCLK clock used as ADC clock source */
      adc_frequency = RCC_GetSystemClockFreq();
      break;

    case LL_RCC_ADC_CLKSOURCE_NONE:          /* No clock used as ADC clock source */
    default:
      adc_frequency = LL_RCC_PERIPH_FREQUENCY_NA;
      break;
  }

  return adc_frequency;
}

/**
  * @brief  Return SWPMIx clock frequency
  * @param  SWPMIxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SWPMI1_CLKSOURCE
  * @retval SWPMI clock frequency (in Hz)
  *         - @ref  LL_RCC_PERIPH_FREQUENCY_NO indicates that oscillator (HSI) is not ready
  */
uint32_t LL_RCC_GetSWPMIClockFreq(uint32_t SWPMIxSource)
{
  uint32_t swpmi_frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_SWPMI_CLKSOURCE(SWPMIxSource));

  /* SWPMI1CLK clock frequency */
  switch (LL_RCC_GetSWPMIClockSource(SWPMIxSource))
  {
    case LL_RCC_SWPMI1_CLKSOURCE_HSI:        /* SWPMI1 Clock is HSI Osc. */
      if (LL_RCC_HSI_IsReady())
      {
        swpmi_frequency = HSI_VALUE;
      }
      break;

    case LL_RCC_SWPMI1_CLKSOURCE_PCLK:       /* SWPMI1 Clock is PCLK1 */
    default:
      swpmi_frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
      break;
  }

  return swpmi_frequency;
}

#if defined(DFSDM_Channel0)
/**
  * @brief  Return DFSDMx clock frequency
  * @param  DFSDMxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_DFSDM_CLKSOURCE
  * @retval DFSDM clock frequency (in Hz)
  */
uint32_t LL_RCC_GetDFSDMClockFreq(uint32_t DFSDMxSource)
{
  uint32_t dfsdm_frequency = LL_RCC_PERIPH_FREQUENCY_NO;

  /* Check parameter */
  assert_param(IS_LL_RCC_DFSDM_CLKSOURCE(DFSDMxSource));

  /* DFSDMCLK clock frequency */
  switch (LL_RCC_GetDFSDMClockSource(DFSDMxSource))
  {
    case LL_RCC_DFSDM_CLKSOURCE_SYSCLK:      /* DFSDM Clock is SYSCLK */
      dfsdm_frequency = RCC_GetSystemClockFreq();
      break;

    case LL_RCC_DFSDM_CLKSOURCE_PCLK:        /* DFSDM Clock is PCLK1 */
    default:
      dfsdm_frequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq()));
      break;
  }

  return dfsdm_frequency;
}
#endif /* DFSDM_Channel0 */

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
  * @brief  Return SYSTEM clock frequency
  * @retval SYSTEM clock frequency (in Hz)
  */
uint32_t RCC_GetSystemClockFreq(void)
{
  uint32_t frequency = 0;

  /* Get SYSCLK source -------------------------------------------------------*/
  switch (LL_RCC_GetSysClkSource())
  {
    case LL_RCC_SYS_CLKSOURCE_STATUS_MSI:  /* MSI used as system clock source */
      frequency = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;

    case LL_RCC_SYS_CLKSOURCE_STATUS_HSI:  /* HSI used as system clock  source */
      frequency = HSI_VALUE;
      break;

    case LL_RCC_SYS_CLKSOURCE_STATUS_HSE:  /* HSE used as system clock  source */
      frequency = HSE_VALUE;
      break;

    case LL_RCC_SYS_CLKSOURCE_STATUS_PLL:  /* PLL used as system clock  source */
      frequency = RCC_PLL_GetFreqDomain_SYS();
      break;

    default:
      frequency = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;
  }

  return frequency;
}

/**
  * @brief  Return HCLK clock frequency
  * @param  SYSCLK_Frequency SYSCLK clock frequency
  * @retval HCLK clock frequency (in Hz)
  */
uint32_t RCC_GetHCLKClockFreq(uint32_t SYSCLK_Frequency)
{
  /* HCLK clock frequency */
  return __LL_RCC_CALC_HCLK_FREQ(SYSCLK_Frequency, LL_RCC_GetAHBPrescaler());
}

/**
  * @brief  Return PCLK1 clock frequency
  * @param  HCLK_Frequency HCLK clock frequency
  * @retval PCLK1 clock frequency (in Hz)
  */
uint32_t RCC_GetPCLK1ClockFreq(uint32_t HCLK_Frequency)
{
  /* PCLK1 clock frequency */
  return __LL_RCC_CALC_PCLK1_FREQ(HCLK_Frequency, LL_RCC_GetAPB1Prescaler());
}

/**
  * @brief  Return PCLK2 clock frequency
  * @param  HCLK_Frequency HCLK clock frequency
  * @retval PCLK2 clock frequency (in Hz)
  */
uint32_t RCC_GetPCLK2ClockFreq(uint32_t HCLK_Frequency)
{
  /* PCLK2 clock frequency */
  return __LL_RCC_CALC_PCLK2_FREQ(HCLK_Frequency, LL_RCC_GetAPB2Prescaler());
}

/**
  * @brief  Return PLL clock frequency used for system domain
  * @retval PLL clock frequency (in Hz)
  */
uint32_t RCC_PLL_GetFreqDomain_SYS(void)
{
  uint32_t pllinputfreq = 0, pllsource = 0;

  /* PLL_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE/ PLLM) * PLLN
     SYSCLK = PLL_VCO / PLLR
  */
  pllsource = LL_RCC_PLL_GetMainSource();

  switch (pllsource)
  {
    case LL_RCC_PLLSOURCE_MSI:  /* MSI used as PLL clock source */
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;

    case LL_RCC_PLLSOURCE_HSI:  /* HSI used as PLL clock source */
      pllinputfreq = HSI_VALUE;
      break;

    case LL_RCC_PLLSOURCE_HSE:  /* HSE used as PLL clock source */
      pllinputfreq = HSE_VALUE;
      break;

    default:
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;
  }
  return __LL_RCC_CALC_PLLCLK_FREQ(pllinputfreq, LL_RCC_PLL_GetDivider(),
                                        LL_RCC_PLL_GetN(), LL_RCC_PLL_GetR());
}

/**
  * @brief  Return PLL clock frequency used for SAI domain
  * @retval PLL clock frequency (in Hz)
  */
uint32_t RCC_PLL_GetFreqDomain_SAI(void)
{
  uint32_t pllinputfreq = 0, pllsource = 0;

  /* PLL_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE / PLLM) * PLLN
     SAI Domain clock = PLL_VCO / PLLP
  */
  pllsource = LL_RCC_PLL_GetMainSource();

  switch (pllsource)
  {
    case LL_RCC_PLLSOURCE_MSI:  /* MSI used as PLL clock source */
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;

    case LL_RCC_PLLSOURCE_HSI:  /* HSI used as PLL clock source */
      pllinputfreq = HSI_VALUE;
      break;

    case LL_RCC_PLLSOURCE_HSE:  /* HSE used as PLL clock source */
      pllinputfreq = HSE_VALUE;
      break;

    default:
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;
  }
  return __LL_RCC_CALC_PLLCLK_SAI_FREQ(pllinputfreq, LL_RCC_PLL_GetDivider(),
                                        LL_RCC_PLL_GetN(), LL_RCC_PLL_GetP());
}

/**
  * @brief  Return PLL clock frequency used for 48 MHz domain
  * @retval PLL clock frequency (in Hz)
  */
uint32_t RCC_PLL_GetFreqDomain_48M(void)
{
  uint32_t pllinputfreq = 0, pllsource = 0;

  /* PLL_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE/ PLLM) * PLLN
     48M Domain clock = PLL_VCO / PLLQ
  */
  pllsource = LL_RCC_PLL_GetMainSource();

  switch (pllsource)
  {
    case LL_RCC_PLLSOURCE_MSI:  /* MSI used as PLL clock source */
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;

    case LL_RCC_PLLSOURCE_HSI:  /* HSI used as PLL clock source */
      pllinputfreq = HSI_VALUE;
      break;

    case LL_RCC_PLLSOURCE_HSE:  /* HSE used as PLL clock source */
      pllinputfreq = HSE_VALUE;
      break;

    default:
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;
  }
  return __LL_RCC_CALC_PLLCLK_48M_FREQ(pllinputfreq, LL_RCC_PLL_GetDivider(),
                                        LL_RCC_PLL_GetN(), LL_RCC_PLL_GetQ());
}

/**
  * @brief  Return PLLSAI1 clock frequency used for SAI domain
  * @retval PLLSAI1 clock frequency (in Hz)
  */
uint32_t RCC_PLLSAI1_GetFreqDomain_SAI(void)
{
  uint32_t pllinputfreq = 0, pllsource = 0;

  /* PLLSAI1_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE/ PLLM) * PLLSAI1N
     SAI Domain clock  = PLLSAI1_VCO / PLLSAI1P
  */
  pllsource = LL_RCC_PLL_GetMainSource();

  switch (pllsource)
  {
    case LL_RCC_PLLSOURCE_MSI:  /* MSI used as PLLSAI1 clock source */
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;

    case LL_RCC_PLLSOURCE_HSI:  /* HSI used as PLLSAI1 clock source */
      pllinputfreq = HSI_VALUE;
      break;

    case LL_RCC_PLLSOURCE_HSE:  /* HSE used as PLLSAI1 clock source */
      pllinputfreq = HSE_VALUE;
      break;

    default:
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;
  }
  return __LL_RCC_CALC_PLLSAI1_SAI_FREQ(pllinputfreq, LL_RCC_PLL_GetDivider(),
                                        LL_RCC_PLLSAI1_GetN(), LL_RCC_PLLSAI1_GetP());
}

/**
  * @brief  Return PLLSAI1 clock frequency used for 48Mhz domain
  * @retval PLLSAI1 clock frequency (in Hz)
  */
uint32_t RCC_PLLSAI1_GetFreqDomain_48M(void)
{
  uint32_t pllinputfreq = 0, pllsource = 0;

  /* PLLSAI1_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE/ PLLM) * PLLSAI1N
     48M Domain clock  = PLLSAI1_VCO / PLLSAI1Q
  */
  pllsource = LL_RCC_PLL_GetMainSource();

  switch (pllsource)
  {
    case LL_RCC_PLLSOURCE_MSI:  /* MSI used as PLLSAI1 clock source */
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;

    case LL_RCC_PLLSOURCE_HSI:  /* HSI used as PLLSAI1 clock source */
      pllinputfreq = HSI_VALUE;
      break;

    case LL_RCC_PLLSOURCE_HSE:  /* HSE used as PLLSAI1 clock source */
      pllinputfreq = HSE_VALUE;
      break;

    default:
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;
  }
  return __LL_RCC_CALC_PLLSAI1_48M_FREQ(pllinputfreq, LL_RCC_PLL_GetDivider(),
                                        LL_RCC_PLLSAI1_GetN(), LL_RCC_PLLSAI1_GetQ());
}

/**
  * @brief  Return PLLSAI1 clock frequency used for ADC domain
  * @retval PLLSAI1 clock frequency (in Hz)
  */
uint32_t RCC_PLLSAI1_GetFreqDomain_ADC(void)
{
  uint32_t pllinputfreq = 0, pllsource = 0;

  /* PLLSAI1_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE/ PLLM) * PLLSAI1N
     48M Domain clock  = PLLSAI1_VCO / PLLSAI1R
  */
  pllsource = LL_RCC_PLL_GetMainSource();

  switch (pllsource)
  {
    case LL_RCC_PLLSOURCE_MSI:  /* MSI used as PLLSAI1 clock source */
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;

    case LL_RCC_PLLSOURCE_HSI:  /* HSI used as PLLSAI1 clock source */
      pllinputfreq = HSI_VALUE;
      break;

    case LL_RCC_PLLSOURCE_HSE:  /* HSE used as PLLSAI1 clock source */
      pllinputfreq = HSE_VALUE;
      break;

    default:
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;
  }
  return __LL_RCC_CALC_PLLSAI1_ADC_FREQ(pllinputfreq, LL_RCC_PLL_GetDivider(),
                                        LL_RCC_PLLSAI1_GetN(), LL_RCC_PLLSAI1_GetR());
}

#if defined(RCC_PLLSAI2_SUPPORT)
/**
  * @brief  Return PLLSAI2 clock frequency used for SAI domain
  * @retval PLLSAI2 clock frequency (in Hz)
  */
uint32_t RCC_PLLSAI2_GetFreqDomain_SAI(void)
{
  uint32_t pllinputfreq = 0, pllsource = 0;

  /* PLLSAI2_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE/ PLLM) * PLLSAI2N
     SAI Domain clock  = PLLSAI2_VCO / PLLSAI2P
  */
  pllsource = LL_RCC_PLL_GetMainSource();

  switch (pllsource)
  {
    case LL_RCC_PLLSOURCE_MSI:  /* MSI used as PLLSAI2 clock source */
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;

    case LL_RCC_PLLSOURCE_HSI:  /* HSI used as PLLSAI2 clock source */
      pllinputfreq = HSI_VALUE;
      break;

    case LL_RCC_PLLSOURCE_HSE:  /* HSE used as PLLSAI2 clock source */
      pllinputfreq = HSE_VALUE;
      break;

    default:
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;
  }
  return __LL_RCC_CALC_PLLSAI2_SAI_FREQ(pllinputfreq, LL_RCC_PLL_GetDivider(),
                                        LL_RCC_PLLSAI2_GetN(), LL_RCC_PLLSAI2_GetP());
}

/**
  * @brief  Return PLLSAI2 clock frequency used for ADC domain
  * @retval PLLSAI2 clock frequency (in Hz)
  */
uint32_t RCC_PLLSAI2_GetFreqDomain_ADC(void)
{
  uint32_t pllinputfreq = 0, pllsource = 0;

  /* PLLSAI2_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE/ PLLM) * PLLSAI2N
     48M Domain clock  = PLLSAI2_VCO / PLLSAI2R
  */
  pllsource = LL_RCC_PLL_GetMainSource();

  switch (pllsource)
  {
    case LL_RCC_PLLSOURCE_MSI:  /* MSI used as PLLSAI2 clock source */
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;

    case LL_RCC_PLLSOURCE_HSI:  /* HSI used as PLLSAI2 clock source */
      pllinputfreq = HSI_VALUE;
      break;

    case LL_RCC_PLLSOURCE_HSE:  /* HSE used as PLLSAI2 clock source */
      pllinputfreq = HSE_VALUE;
      break;

    default:
      pllinputfreq = __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
                                    (LL_RCC_MSI_IsEnabledRangeSelect() ?
                                     LL_RCC_MSI_GetRange() :
                                     LL_RCC_MSI_GetRangeAfterStandby()));
      break;
  }
  return __LL_RCC_CALC_PLLSAI2_ADC_FREQ(pllinputfreq, LL_RCC_PLL_GetDivider(),
                                        LL_RCC_PLLSAI2_GetN(), LL_RCC_PLLSAI2_GetR());
}

#endif /*RCC_PLLSAI2_SUPPORT*/
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

#endif /* USE_FULL_LL_DRIVER */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
