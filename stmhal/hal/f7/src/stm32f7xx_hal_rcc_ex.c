/**
  ******************************************************************************
  * @file    stm32f7xx_hal_rcc_ex.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-June-2015
  * @brief   Extension RCC HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities RCC extension peripheral:
  *           + Extended Peripheral Control functions
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @defgroup RCCEx RCCEx
  * @brief RCCEx HAL module driver
  * @{
  */

#ifdef HAL_RCC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup RCCEx_Private_Defines RCCEx Private Defines
  * @{
  */

#define PLLI2S_TIMEOUT_VALUE    100 /* Timeout value fixed to 100 ms  */
#define PLLSAI_TIMEOUT_VALUE    100 /* Timeout value fixed to 100 ms  */

/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/
/** @defgroup RCCEx_Private_Macros RCCEx Private Macros
 * @{
 */
/**
  * @}
  */

/** @defgroup RCCEx_Private_Macros RCCEx Private Macros
 * @{
 */

/**
  * @}
  */


/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup RCCEx_Exported_Functions RCCEx Exported Functions
  * @{
  */

/** @defgroup RCCEx_Exported_Functions_Group1 Extended Peripheral Control functions
 *  @brief  Extended Peripheral Control functions
 *
@verbatim
 ===============================================================================
                ##### Extended Peripheral Control functions  #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the RCC Clocks
    frequencies.
    [..]
    (@) Important note: Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to
        select the RTC clock source; in this case the Backup domain will be reset in
        order to modify the RTC Clock source, as consequence RTC registers (including
        the backup registers) and RCC_BDCR register will be set to their reset values.

@endverbatim
  * @{
  */
/**
  * @brief  Initializes the RCC extended peripherals clocks according to the specified
  *         parameters in the RCC_PeriphCLKInitTypeDef.
  * @param  PeriphClkInit: pointer to an RCC_PeriphCLKInitTypeDef structure that
  *         contains the configuration information for the Extended Peripherals
  *         clocks(I2S, SAI, LTDC RTC, TIM, UARTs, USARTs, LTPIM, SDMMC...).
  *
  * @note   Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
  *         the RTC clock source; in this case the Backup domain will be reset in
  *         order to modify the RTC Clock source, as consequence RTC registers (including
  *         the backup registers) and RCC_BDCR register are set to their reset values.
  *
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit)
{
  uint32_t tickstart = 0;
  uint32_t tmpreg0 = 0;
  uint32_t tmpreg1 = 0;
  uint32_t plli2sused = 0;
  uint32_t pllsaiused = 0;

  /* Check the parameters */
  assert_param(IS_RCC_PERIPHCLOCK(PeriphClkInit->PeriphClockSelection));

  /*----------------------------------- I2S configuration ----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2S) == (RCC_PERIPHCLK_I2S))
  {
    /* Check the parameters */
    assert_param(IS_RCC_I2SCLKSOURCE(PeriphClkInit->I2sClockSelection));

    /* Configure I2S Clock source */
    __HAL_RCC_I2S_CONFIG(PeriphClkInit->I2sClockSelection);

    /* Enable the PLLI2S when it's used as clock source for I2S */
    if(PeriphClkInit->I2sClockSelection == RCC_I2SCLKSOURCE_PLLI2S)
    {
      plli2sused = 1;
    }
  }

  /*------------------------------------ SAI1 configuration --------------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI1) == (RCC_PERIPHCLK_SAI1))
  {
    /* Check the parameters */
    assert_param(IS_RCC_SAI1CLKSOURCE(PeriphClkInit->Sai1ClockSelection));

    /* Configure SAI1 Clock source */
    __HAL_RCC_SAI1_CONFIG(PeriphClkInit->Sai1ClockSelection);
    /* Enable the PLLI2S when it's used as clock source for SAI */
    if(PeriphClkInit->Sai1ClockSelection == RCC_SAI1CLKSOURCE_PLLI2S)
    {
      plli2sused = 1;
    }
    /* Enable the PLLSAI when it's used as clock source for SAI */
    if(PeriphClkInit->Sai1ClockSelection == RCC_SAI1CLKSOURCE_PLLSAI)
    {
      pllsaiused = 1;
    }
  }

  /*------------------------------------ SAI2 configuration --------------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI2) == (RCC_PERIPHCLK_SAI2))
  {
    /* Check the parameters */
    assert_param(IS_RCC_SAI2CLKSOURCE(PeriphClkInit->Sai2ClockSelection));

    /* Configure SAI2 Clock source */
    __HAL_RCC_SAI2_CONFIG(PeriphClkInit->Sai2ClockSelection);

    /* Enable the PLLI2S when it's used as clock source for SAI */
    if(PeriphClkInit->Sai2ClockSelection == RCC_SAI2CLKSOURCE_PLLI2S)
    {
      plli2sused = 1;
    }
    /* Enable the PLLSAI when it's used as clock source for SAI */
    if(PeriphClkInit->Sai2ClockSelection == RCC_SAI2CLKSOURCE_PLLSAI)
    {
      pllsaiused = 1;
    }
  }

  /*-------------------------------------- SPDIF-RX Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SPDIFRX) == RCC_PERIPHCLK_SPDIFRX)
  {
      plli2sused = 1;
  }

  /*------------------------------------ RTC configuration --------------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_RTC) == (RCC_PERIPHCLK_RTC))
  {
    /* Reset the Backup domain only if the RTC Clock source selection is modified */
    if((RCC->BDCR & RCC_BDCR_RTCSEL) != (PeriphClkInit->RTCClockSelection & RCC_BDCR_RTCSEL))
    {
      /* Enable Power Clock*/
      __HAL_RCC_PWR_CLK_ENABLE();

      /* Enable write access to Backup domain */
      PWR->CR1 |= PWR_CR1_DBP;

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait for Backup domain Write protection disable */
      while((PWR->CR1 & PWR_CR1_DBP) == RESET)
      {
        if((HAL_GetTick() - tickstart) > RCC_DBP_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }

      /* Store the content of BDCR register before the reset of Backup Domain */
      tmpreg0 = (RCC->BDCR & ~(RCC_BDCR_RTCSEL));

      /* RTC Clock selection can be changed only if the Backup Domain is reset */
      __HAL_RCC_BACKUPRESET_FORCE();
      __HAL_RCC_BACKUPRESET_RELEASE();

      /* Restore the Content of BDCR register */
      RCC->BDCR = tmpreg0;

      /* If LSE is selected as RTC clock source, wait for LSE reactivation */
      if (HAL_IS_BIT_SET(tmpreg0, RCC_BDCR_LSERDY))
      {
        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till LSE is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)
        {
          if((HAL_GetTick() - tickstart ) > RCC_LSE_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
      __HAL_RCC_RTC_CONFIG(PeriphClkInit->RTCClockSelection);
    }
  }

  /*------------------------------------ TIM configuration --------------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_TIM) == (RCC_PERIPHCLK_TIM))
  {
    /* Check the parameters */
    assert_param(IS_RCC_TIMPRES(PeriphClkInit->TIMPresSelection));

    /* Configure Timer Prescaler */
    __HAL_RCC_TIMCLKPRESCALER(PeriphClkInit->TIMPresSelection);
  }

  /*-------------------------------------- I2C1 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2C1) == RCC_PERIPHCLK_I2C1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_I2C1CLKSOURCE(PeriphClkInit->I2c1ClockSelection));

    /* Configure the I2C1 clock source */
    __HAL_RCC_I2C1_CONFIG(PeriphClkInit->I2c1ClockSelection);
  }

  /*-------------------------------------- I2C2 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2C2) == RCC_PERIPHCLK_I2C2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_I2C2CLKSOURCE(PeriphClkInit->I2c2ClockSelection));

    /* Configure the I2C2 clock source */
    __HAL_RCC_I2C2_CONFIG(PeriphClkInit->I2c2ClockSelection);
  }

  /*-------------------------------------- I2C3 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2C3) == RCC_PERIPHCLK_I2C3)
  {
    /* Check the parameters */
    assert_param(IS_RCC_I2C3CLKSOURCE(PeriphClkInit->I2c3ClockSelection));

    /* Configure the I2C3 clock source */
    __HAL_RCC_I2C3_CONFIG(PeriphClkInit->I2c3ClockSelection);
  }

  /*-------------------------------------- I2C4 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2C4) == RCC_PERIPHCLK_I2C4)
  {
    /* Check the parameters */
    assert_param(IS_RCC_I2C4CLKSOURCE(PeriphClkInit->I2c4ClockSelection));

    /* Configure the I2C4 clock source */
    __HAL_RCC_I2C4_CONFIG(PeriphClkInit->I2c4ClockSelection);
  }

  /*-------------------------------------- USART1 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_USART1) == RCC_PERIPHCLK_USART1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USART1CLKSOURCE(PeriphClkInit->Usart1ClockSelection));

    /* Configure the USART1 clock source */
    __HAL_RCC_USART1_CONFIG(PeriphClkInit->Usart1ClockSelection);
  }

  /*-------------------------------------- USART2 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_USART2) == RCC_PERIPHCLK_USART2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USART2CLKSOURCE(PeriphClkInit->Usart2ClockSelection));

    /* Configure the USART2 clock source */
    __HAL_RCC_USART2_CONFIG(PeriphClkInit->Usart2ClockSelection);
  }

  /*-------------------------------------- USART3 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_USART3) == RCC_PERIPHCLK_USART3)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USART3CLKSOURCE(PeriphClkInit->Usart3ClockSelection));

    /* Configure the USART3 clock source */
    __HAL_RCC_USART3_CONFIG(PeriphClkInit->Usart3ClockSelection);
  }

  /*-------------------------------------- UART4 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_UART4) == RCC_PERIPHCLK_UART4)
  {
    /* Check the parameters */
    assert_param(IS_RCC_UART4CLKSOURCE(PeriphClkInit->Uart4ClockSelection));

    /* Configure the UART4 clock source */
    __HAL_RCC_UART4_CONFIG(PeriphClkInit->Uart4ClockSelection);
  }

  /*-------------------------------------- UART5 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_UART5) == RCC_PERIPHCLK_UART5)
  {
    /* Check the parameters */
    assert_param(IS_RCC_UART5CLKSOURCE(PeriphClkInit->Uart5ClockSelection));

    /* Configure the UART5 clock source */
    __HAL_RCC_UART5_CONFIG(PeriphClkInit->Uart5ClockSelection);
  }

  /*-------------------------------------- USART6 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_USART6) == RCC_PERIPHCLK_USART6)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USART6CLKSOURCE(PeriphClkInit->Usart6ClockSelection));

    /* Configure the USART6 clock source */
    __HAL_RCC_USART6_CONFIG(PeriphClkInit->Usart6ClockSelection);
  }

  /*-------------------------------------- UART7 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_UART7) == RCC_PERIPHCLK_UART7)
  {
    /* Check the parameters */
    assert_param(IS_RCC_UART7CLKSOURCE(PeriphClkInit->Uart7ClockSelection));

    /* Configure the UART7 clock source */
    __HAL_RCC_UART7_CONFIG(PeriphClkInit->Uart7ClockSelection);
  }

  /*-------------------------------------- UART8 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_UART8) == RCC_PERIPHCLK_UART8)
  {
    /* Check the parameters */
    assert_param(IS_RCC_UART8CLKSOURCE(PeriphClkInit->Uart8ClockSelection));

    /* Configure the UART8 clock source */
    __HAL_RCC_UART8_CONFIG(PeriphClkInit->Uart8ClockSelection);
  }

  /*--------------------------------------- CEC Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_CEC) == RCC_PERIPHCLK_CEC)
  {
    /* Check the parameters */
    assert_param(IS_RCC_CECCLKSOURCE(PeriphClkInit->CecClockSelection));

    /* Configure the CEC clock source */
    __HAL_RCC_CEC_CONFIG(PeriphClkInit->CecClockSelection);
  }

  /*-------------------------------------- CK48 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_CLK48) == RCC_PERIPHCLK_CLK48)
  {
    /* Check the parameters */
    assert_param(IS_RCC_CLK48SOURCE(PeriphClkInit->Clk48ClockSelection));

    /* Configure the CLK48 source */
    __HAL_RCC_CLK48_CONFIG(PeriphClkInit->Clk48ClockSelection);

    /* Enable the PLLSAI when it's used as clock source for CK48 */
    if(PeriphClkInit->Clk48ClockSelection == RCC_CLK48SOURCE_PLLSAIP)
    {
      pllsaiused = 1;
    }
  }

  /*-------------------------------------- LTDC Configuration -----------------------------------*/
#if defined(STM32F756xx) || defined(STM32F746xx)
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LTDC) == RCC_PERIPHCLK_LTDC)
  {
    pllsaiused = 1;
  }
#endif /* STM32F756xx || STM32F746xx */
  /*-------------------------------------- LPTIM1 Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LPTIM1) == RCC_PERIPHCLK_LPTIM1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LPTIM1CLK(PeriphClkInit->Lptim1ClockSelection));

    /* Configure the LTPIM1 clock source */
    __HAL_RCC_LPTIM1_CONFIG(PeriphClkInit->Lptim1ClockSelection);
   }

  /*------------------------------------- SDMMC Configuration ------------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SDMMC1) == RCC_PERIPHCLK_SDMMC1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SDMMC1CLKSOURCE(PeriphClkInit->Sdmmc1ClockSelection));

    /* Configure the SDMMC1 clock source */
    __HAL_RCC_SDMMC1_CONFIG(PeriphClkInit->Sdmmc1ClockSelection);
  }

  /*-------------------------------------- PLLI2S Configuration ---------------------------------*/
  /* PLLI2S is configured when a peripheral will use it as source clock : SAI1, SAI2, I2S or SPDIF-RX */
  if((plli2sused == 1) || (PeriphClkInit->PeriphClockSelection == RCC_PERIPHCLK_PLLI2S))
  {
    /* Disable the PLLI2S */
    __HAL_RCC_PLLI2S_DISABLE();

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    /* Wait till PLLI2S is disabled */
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLI2SRDY)  != RESET)
    {
      if((HAL_GetTick() - tickstart) > PLLI2S_TIMEOUT_VALUE)
      {
        /* return in case of Timeout detected */
        return HAL_TIMEOUT;
      }
    }

    /* check for common PLLI2S Parameters */
    assert_param(IS_RCC_PLLI2SN_VALUE(PeriphClkInit->PLLI2S.PLLI2SN));

    /*----------------- In Case of PLLI2S is selected as source clock for I2S -------------------*/
    if(((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2S) == RCC_PERIPHCLK_I2S) && (PeriphClkInit->I2sClockSelection == RCC_I2SCLKSOURCE_PLLI2S)))
    {
      /* check for Parameters */
      assert_param(IS_RCC_PLLI2SR_VALUE(PeriphClkInit->PLLI2S.PLLI2SR));

      /* Read PLLI2SP and PLLI2SQ value from PLLI2SCFGR register (this value is not needed for I2S configuration) */
      tmpreg0 = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SP) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SP));
      tmpreg1 = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SQ) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SQ));
      /* Configure the PLLI2S division factors */
      /* PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input) x (PLLI2SN/PLLM) */
      /* I2SCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SR */
      __HAL_RCC_PLLI2S_CONFIG(PeriphClkInit->PLLI2S.PLLI2SN , tmpreg0, tmpreg1, PeriphClkInit->PLLI2S.PLLI2SR);
    }

    /*----------------- In Case of PLLI2S is selected as source clock for SAI -------------------*/
    if(((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI1) == RCC_PERIPHCLK_SAI1) && (PeriphClkInit->Sai1ClockSelection == RCC_SAI1CLKSOURCE_PLLI2S)) ||
       ((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI2) == RCC_PERIPHCLK_SAI2) && (PeriphClkInit->Sai2ClockSelection == RCC_SAI2CLKSOURCE_PLLI2S)))
    {
      /* Check for PLLI2S Parameters */
      assert_param(IS_RCC_PLLI2SQ_VALUE(PeriphClkInit->PLLI2S.PLLI2SQ));
      /* Check for PLLI2S/DIVQ parameters */
      assert_param(IS_RCC_PLLI2S_DIVQ_VALUE(PeriphClkInit->PLLI2SDivQ));

      /* Read PLLI2SP and PLLI2SR values from PLLI2SCFGR register (this value is not needed for SAI configuration) */
      tmpreg0 = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SP) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SP));
      tmpreg1 = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SR));
      /* Configure the PLLI2S division factors */
      /* PLLI2S_VCO Input  = PLL_SOURCE/PLLM */
      /* PLLI2S_VCO Output = PLLI2S_VCO Input * PLLI2SN */
      /* SAI_CLK(first level) = PLLI2S_VCO Output/PLLI2SQ */
      __HAL_RCC_PLLI2S_CONFIG(PeriphClkInit->PLLI2S.PLLI2SN, tmpreg0, PeriphClkInit->PLLI2S.PLLI2SQ, tmpreg1);

      /* SAI_CLK_x = SAI_CLK(first level)/PLLI2SDIVQ */
      __HAL_RCC_PLLI2S_PLLSAICLKDIVQ_CONFIG(PeriphClkInit->PLLI2SDivQ);
    }

    /*----------------- In Case of PLLI2S is selected as source clock for SPDIF-RX -------------------*/
    if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SPDIFRX) == RCC_PERIPHCLK_SPDIFRX)
    {
      /* check for Parameters */
      assert_param(IS_RCC_PLLI2SP_VALUE(PeriphClkInit->PLLI2S.PLLI2SP));

     /* Read PLLI2SR value from PLLI2SCFGR register (this value is not needed for SPDIF-RX configuration) */
      tmpreg0 = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SQ) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SQ));
      tmpreg1 = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SR));
      /* Configure the PLLI2S division factors */
      /* PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input) x (PLLI2SN/PLLM) */
      /* SPDIFCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SP */
      __HAL_RCC_PLLI2S_CONFIG(PeriphClkInit->PLLI2S.PLLI2SN , PeriphClkInit->PLLI2S.PLLI2SP, tmpreg0, tmpreg1);
    }

    /*----------------- In Case of PLLI2S is just selected  -----------------*/
    if((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_PLLI2S) == RCC_PERIPHCLK_PLLI2S)
    {
      /* Check for Parameters */
      assert_param(IS_RCC_PLLI2SN_VALUE(PeriphClkInit->PLLI2S.PLLI2SN));
      assert_param(IS_RCC_PLLI2SP_VALUE(PeriphClkInit->PLLI2S.PLLI2SP));
      assert_param(IS_RCC_PLLI2SR_VALUE(PeriphClkInit->PLLI2S.PLLI2SR));
      assert_param(IS_RCC_PLLI2SQ_VALUE(PeriphClkInit->PLLI2S.PLLI2SQ));

      /* Configure the PLLI2S division factors */
      /* PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input) x (PLLI2SN/PLLI2SM) */
      /* SPDIFRXCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SP */
      __HAL_RCC_PLLI2S_CONFIG(PeriphClkInit->PLLI2S.PLLI2SN , PeriphClkInit->PLLI2S.PLLI2SP, PeriphClkInit->PLLI2S.PLLI2SQ, PeriphClkInit->PLLI2S.PLLI2SR);
    }

    /* Enable the PLLI2S */
    __HAL_RCC_PLLI2S_ENABLE();

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    /* Wait till PLLI2S is ready */
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLI2SRDY)  == RESET)
    {
      if((HAL_GetTick() - tickstart) > PLLI2S_TIMEOUT_VALUE)
      {
        /* return in case of Timeout detected */
        return HAL_TIMEOUT;
      }
    }
  }

  /*-------------------------------------- PLLSAI Configuration ---------------------------------*/
  /* PLLSAI is configured when a peripheral will use it as source clock : SAI1, SAI2, LTDC or CK48 */
  if(pllsaiused == 1)
  {
    /* Disable PLLSAI Clock */
    __HAL_RCC_PLLSAI_DISABLE();

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    /* Wait till PLLSAI is disabled */
    while(__HAL_RCC_PLLSAI_GET_FLAG() != RESET)
    {
      if((HAL_GetTick() - tickstart) > PLLSAI_TIMEOUT_VALUE)
      {
        /* return in case of Timeout detected */
        return HAL_TIMEOUT;
      }
    }

    /* Check the PLLSAI division factors */
    assert_param(IS_RCC_PLLSAIN_VALUE(PeriphClkInit->PLLSAI.PLLSAIN));

    /*----------------- In Case of PLLSAI is selected as source clock for SAI -------------------*/
    if(((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI1) == RCC_PERIPHCLK_SAI1) && (PeriphClkInit->Sai1ClockSelection == RCC_SAI1CLKSOURCE_PLLSAI)) ||
       ((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI2) == RCC_PERIPHCLK_SAI2) && (PeriphClkInit->Sai2ClockSelection == RCC_SAI2CLKSOURCE_PLLSAI)))
    {
      /* check for PLLSAIQ Parameter */
      assert_param(IS_RCC_PLLSAIQ_VALUE(PeriphClkInit->PLLSAI.PLLSAIQ));
      /* check for PLLSAI/DIVQ Parameter */
      assert_param(IS_RCC_PLLSAI_DIVQ_VALUE(PeriphClkInit->PLLSAIDivQ));

      /* Read PLLSAIP value from PLLSAICFGR register (this value is not needed for SAI configuration) */
      tmpreg0 = ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIP) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIP));
      tmpreg1 = ((RCC->PLLSAICFGR & RCC_PLLI2SCFGR_PLLI2SR) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIR));
      /* PLLSAI_VCO Input  = PLL_SOURCE/PLLM */
      /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN */
      /* SAI_CLK(first level) = PLLSAI_VCO Output/PLLSAIQ */
      __HAL_RCC_PLLSAI_CONFIG(PeriphClkInit->PLLSAI.PLLSAIN , tmpreg0, PeriphClkInit->PLLSAI.PLLSAIQ, tmpreg1);

      /* SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ */
      __HAL_RCC_PLLSAI_PLLSAICLKDIVQ_CONFIG(PeriphClkInit->PLLSAIDivQ);
    }

    /*----------------- In Case of PLLSAI is selected as source clock for CLK48 -------------------*/
    /* In Case of PLLI2S is selected as source clock for CK48 */
    if((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_CLK48) == RCC_PERIPHCLK_CLK48) && (PeriphClkInit->Clk48ClockSelection == RCC_CLK48SOURCE_PLLSAIP))
    {
      /* check for Parameters */
      assert_param(IS_RCC_PLLSAIP_VALUE(PeriphClkInit->PLLSAI.PLLSAIP));
      /* Read PLLSAIQ and PLLSAIR value from PLLSAICFGR register (this value is not needed for CK48 configuration) */
      tmpreg0 = ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIQ) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIQ));
      tmpreg1 = ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIR) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIR));

      /* Configure the PLLSAI division factors */
      /* PLLSAI_VCO = f(VCO clock) = f(PLLSAI clock input) x (PLLI2SN/PLLM) */
      /* 48CLK = f(PLLSAI clock output) = f(VCO clock) / PLLSAIP */
      __HAL_RCC_PLLSAI_CONFIG(PeriphClkInit->PLLSAI.PLLSAIN , PeriphClkInit->PLLSAI.PLLSAIP, tmpreg0, tmpreg1);
    }

#if defined(STM32F756xx) || defined(STM32F746xx)
    /*---------------------------- LTDC configuration -------------------------------*/
    if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LTDC) == (RCC_PERIPHCLK_LTDC))
    {
      assert_param(IS_RCC_PLLSAIR_VALUE(PeriphClkInit->PLLSAI.PLLSAIR));
      assert_param(IS_RCC_PLLSAI_DIVR_VALUE(PeriphClkInit->PLLSAIDivR));

      /* Read PLLSAIP and PLLSAIQ value from PLLSAICFGR register (these value are not needed for LTDC configuration) */
      tmpreg0 = ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIQ) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIQ));
      tmpreg1 = ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIP) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIP));

      /* PLLSAI_VCO Input  = PLL_SOURCE/PLLM */
      /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN */
      /* LTDC_CLK(first level) = PLLSAI_VCO Output/PLLSAIR */
      __HAL_RCC_PLLSAI_CONFIG(PeriphClkInit->PLLSAI.PLLSAIN , tmpreg1, tmpreg0, PeriphClkInit->PLLSAI.PLLSAIR);

      /* LTDC_CLK = LTDC_CLK(first level)/PLLSAIDIVR */
      __HAL_RCC_PLLSAI_PLLSAICLKDIVR_CONFIG(PeriphClkInit->PLLSAIDivR);
    }
#endif /* STM32F756xx || STM32F746xx */

    /* Enable PLLSAI Clock */
    __HAL_RCC_PLLSAI_ENABLE();

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    /* Wait till PLLSAI is ready */
    while(__HAL_RCC_PLLSAI_GET_FLAG() == RESET)
    {
      if((HAL_GetTick() - tickstart) > PLLSAI_TIMEOUT_VALUE)
      {
        /* return in case of Timeout detected */
        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  Get the RCC_PeriphCLKInitTypeDef according to the internal
  *         RCC configuration registers.
  * @param  PeriphClkInit: pointer to the configured RCC_PeriphCLKInitTypeDef structure
  * @retval None
  */
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit)
{
  uint32_t tempreg = 0;

  /* Set all possible values for the extended clock type parameter------------*/
  PeriphClkInit->PeriphClockSelection = RCC_PERIPHCLK_I2S      | RCC_PERIPHCLK_LPTIM1 |\
                                        RCC_PERIPHCLK_SAI1     | RCC_PERIPHCLK_SAI2     |\
                                        RCC_PERIPHCLK_TIM      | RCC_PERIPHCLK_RTC      |\
                                        RCC_PERIPHCLK_CEC      | RCC_PERIPHCLK_I2C4     |\
                                        RCC_PERIPHCLK_I2C1     | RCC_PERIPHCLK_I2C2     |\
                                        RCC_PERIPHCLK_I2C3     | RCC_PERIPHCLK_USART1   |\
                                        RCC_PERIPHCLK_USART2   | RCC_PERIPHCLK_USART3   |\
                                        RCC_PERIPHCLK_UART4    | RCC_PERIPHCLK_UART5    |\
                                        RCC_PERIPHCLK_USART6   | RCC_PERIPHCLK_UART7    |\
                                        RCC_PERIPHCLK_UART8    | RCC_PERIPHCLK_SDMMC1    |\
                                        RCC_PERIPHCLK_CLK48;

  /* Get the PLLI2S Clock configuration -----------------------------------------------*/
  PeriphClkInit->PLLI2S.PLLI2SN = (uint32_t)((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SN));
  PeriphClkInit->PLLI2S.PLLI2SP = (uint32_t)((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SP) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SP));
  PeriphClkInit->PLLI2S.PLLI2SQ = (uint32_t)((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SQ) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SQ));
  PeriphClkInit->PLLI2S.PLLI2SR = (uint32_t)((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SR));

  /* Get the PLLSAI Clock configuration -----------------------------------------------*/
  PeriphClkInit->PLLSAI.PLLSAIN = (uint32_t)((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIN) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIN));
  PeriphClkInit->PLLSAI.PLLSAIP = (uint32_t)((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIP) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIP));
  PeriphClkInit->PLLSAI.PLLSAIQ = (uint32_t)((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIQ) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIQ));
  PeriphClkInit->PLLSAI.PLLSAIR = (uint32_t)((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIR) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIR));

  /* Get the PLLSAI/PLLI2S division factors -------------------------------------------*/
  PeriphClkInit->PLLI2SDivQ = (uint32_t)((RCC->DCKCFGR1 & RCC_DCKCFGR1_PLLI2SDIVQ) >> POSITION_VAL(RCC_DCKCFGR1_PLLI2SDIVQ));
  PeriphClkInit->PLLSAIDivQ = (uint32_t)((RCC->DCKCFGR1 & RCC_DCKCFGR1_PLLSAIDIVQ) >> POSITION_VAL(RCC_DCKCFGR1_PLLSAIDIVQ));
  PeriphClkInit->PLLSAIDivR = (uint32_t)((RCC->DCKCFGR1 & RCC_DCKCFGR1_PLLSAIDIVR) >> POSITION_VAL(RCC_DCKCFGR1_PLLSAIDIVR));

  /* Get the SAI1 clock configuration ----------------------------------------------*/
  PeriphClkInit->Sai1ClockSelection = __HAL_RCC_GET_SAI1_SOURCE();

  /* Get the SAI2 clock configuration ----------------------------------------------*/
  PeriphClkInit->Sai2ClockSelection = __HAL_RCC_GET_SAI2_SOURCE();

  /* Get the I2S clock configuration ------------------------------------------*/
  PeriphClkInit->I2sClockSelection = __HAL_RCC_GET_I2SCLKSOURCE();

  /* Get the I2C1 clock configuration ------------------------------------------*/
  PeriphClkInit->I2c1ClockSelection = __HAL_RCC_GET_I2C1_SOURCE();

  /* Get the I2C2 clock configuration ------------------------------------------*/
  PeriphClkInit->I2c2ClockSelection = __HAL_RCC_GET_I2C2_SOURCE();

  /* Get the I2C3 clock configuration ------------------------------------------*/
  PeriphClkInit->I2c3ClockSelection = __HAL_RCC_GET_I2C3_SOURCE();

  /* Get the I2C4 clock configuration ------------------------------------------*/
  PeriphClkInit->I2c4ClockSelection = __HAL_RCC_GET_I2C4_SOURCE();

  /* Get the USART1 clock configuration ------------------------------------------*/
  PeriphClkInit->Usart1ClockSelection = __HAL_RCC_GET_USART1_SOURCE();

  /* Get the USART2 clock configuration ------------------------------------------*/
  PeriphClkInit->Usart2ClockSelection = __HAL_RCC_GET_USART2_SOURCE();

  /* Get the USART3 clock configuration ------------------------------------------*/
  PeriphClkInit->Usart3ClockSelection = __HAL_RCC_GET_USART3_SOURCE();

  /* Get the UART4 clock configuration ------------------------------------------*/
  PeriphClkInit->Uart4ClockSelection = __HAL_RCC_GET_UART4_SOURCE();

  /* Get the UART5 clock configuration ------------------------------------------*/
  PeriphClkInit->Uart5ClockSelection = __HAL_RCC_GET_UART5_SOURCE();

  /* Get the USART6 clock configuration ------------------------------------------*/
  PeriphClkInit->Usart6ClockSelection = __HAL_RCC_GET_USART6_SOURCE();

  /* Get the UART7 clock configuration ------------------------------------------*/
  PeriphClkInit->Uart7ClockSelection = __HAL_RCC_GET_UART7_SOURCE();

  /* Get the UART8 clock configuration ------------------------------------------*/
  PeriphClkInit->Uart8ClockSelection = __HAL_RCC_GET_UART8_SOURCE();

  /* Get the LPTIM1 clock configuration ------------------------------------------*/
  PeriphClkInit->Lptim1ClockSelection = __HAL_RCC_GET_LPTIM1_SOURCE();

  /* Get the CEC clock configuration -----------------------------------------------*/
  PeriphClkInit->CecClockSelection = __HAL_RCC_GET_CEC_SOURCE();

  /* Get the CK48 clock configuration -----------------------------------------------*/
  PeriphClkInit->Clk48ClockSelection = __HAL_RCC_GET_CLK48_SOURCE();

  /* Get the SDMMC clock configuration -----------------------------------------------*/
  PeriphClkInit->Sdmmc1ClockSelection = __HAL_RCC_GET_SDMMC1_SOURCE();

  /* Get the RTC Clock configuration -----------------------------------------------*/
  tempreg = (RCC->CFGR & RCC_CFGR_RTCPRE);
  PeriphClkInit->RTCClockSelection = (uint32_t)((tempreg) | (RCC->BDCR & RCC_BDCR_RTCSEL));

  /* Get the TIM Prescaler configuration --------------------------------------------*/
  if ((RCC->DCKCFGR1 & RCC_DCKCFGR1_TIMPRE) == RESET)
  {
    PeriphClkInit->TIMPresSelection = RCC_TIMPRES_DESACTIVATED;
  }
  else
  {
    PeriphClkInit->TIMPresSelection = RCC_TIMPRES_ACTIVATED;
  }
}

/**
  * @brief  Return the peripheral clock frequency for a given peripheral(SAI..)
  * @note   Return 0 if peripheral clock identifier not managed by this API
  * @param  PeriphClk: Peripheral clock identifier
  *         This parameter can be one of the following values:
  *            @arg RCC_PERIPHCLK_SAI1: SAI1 peripheral clock
  *            @arg RCC_PERIPHCLK_SAI2: SAI2 peripheral clock
  * @retval Frequency in KHz
  */
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t PeriphClk)
{
  uint32_t tmpreg = 0;
  /* This variable used to store the SAI clock frequency (value in Hz) */
  uint32_t frequency = 0;
  /* This variable used to store the VCO Input (value in Hz) */
  uint32_t vcoinput = 0;
  /* This variable used to store the SAI clock source */
  uint32_t saiclocksource = 0;
  if ((PeriphClk == RCC_PERIPHCLK_SAI1) || (PeriphClk == RCC_PERIPHCLK_SAI2))
  {
    saiclocksource = RCC->DCKCFGR1;
    saiclocksource &= (RCC_DCKCFGR1_SAI1SEL | RCC_DCKCFGR1_SAI2SEL);
    switch (saiclocksource)
    {
    case 0: /* PLLSAI is the clock source for SAI*/
      {
        /* Configure the PLLSAI division factor */
        /* PLLSAI_VCO Input  = PLL_SOURCE/PLLM */
        if((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLSOURCE_HSI)
        {
          /* In Case the PLL Source is HSI (Internal Clock) */
          vcoinput = (HSI_VALUE / (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM));
        }
        else
        {
          /* In Case the PLL Source is HSE (External Clock) */
          vcoinput = ((HSE_VALUE / (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM)));
        }
        /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN */
        /* SAI_CLK(first level) = PLLSAI_VCO Output/PLLSAIQ */
        tmpreg = (RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIQ) >> 24;
        frequency = (vcoinput * ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIN) >> 6))/(tmpreg);

        /* SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ */
        tmpreg = (((RCC->DCKCFGR1 & RCC_DCKCFGR1_PLLSAIDIVQ) >> 8) + 1);
        frequency = frequency/(tmpreg);
        break;
      }
    case RCC_DCKCFGR1_SAI1SEL_0: /* PLLI2S is the clock source for SAI*/
    case RCC_DCKCFGR1_SAI2SEL_0: /* PLLI2S is the clock source for SAI*/
      {
        /* Configure the PLLI2S division factor */
        /* PLLI2S_VCO Input  = PLL_SOURCE/PLLM */
        if((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLSOURCE_HSI)
        {
          /* In Case the PLL Source is HSI (Internal Clock) */
          vcoinput = (HSI_VALUE / (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM));
        }
        else
        {
          /* In Case the PLL Source is HSE (External Clock) */
          vcoinput = ((HSE_VALUE / (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM)));
        }

        /* PLLI2S_VCO Output = PLLI2S_VCO Input * PLLI2SN */
        /* SAI_CLK(first level) = PLLI2S_VCO Output/PLLI2SQ */
        tmpreg = (RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SQ) >> 24;
        frequency = (vcoinput * ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN) >> 6))/(tmpreg);

        /* SAI_CLK_x = SAI_CLK(first level)/PLLI2SDIVQ */
        tmpreg = ((RCC->DCKCFGR1 & RCC_DCKCFGR1_PLLI2SDIVQ) + 1);
        frequency = frequency/(tmpreg);
        break;
      }
    case RCC_DCKCFGR1_SAI1SEL_1: /* External clock is the clock source for SAI*/
    case RCC_DCKCFGR1_SAI2SEL_1: /* External clock is the clock source for SAI*/
      {
        frequency = EXTERNAL_CLOCK_VALUE;
        break;
      }
    default :
      {
        break;
      }
    }
  }
  return frequency;
}

/**
  * @}
  */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
