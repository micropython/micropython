/**
  ******************************************************************************
  * @file    stm32f4xx_hal_rcc_ex.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   Extension RCC HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities RCC extension peripheral:
  *           + Extended Peripheral Control functions
  *  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup RCC 
  * @brief RCC HAL module driver
  * @{
  */

#ifdef HAL_RCC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PLLI2S_TIMEOUT_VALUE    100 /* Timeout value fixed to 100 ms  */
#define PLLSAI_TIMEOUT_VALUE    100 /* Timeout value fixed to 100 ms  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup RCCEx_Private_Functions
  * @{
  */

/** @defgroup RCCEx_Group1 Extended Peripheral Control functions 
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
        the backup registers) and RCC_BDCR register are set to their reset values.
      
@endverbatim
  * @{
  */
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx)|| defined(STM32F439xx) 
/**
  * @brief  Initializes the RCC extended peripherals clocks according to the specified
  *         parameters in the RCC_PeriphCLKInitTypeDef.
  * @param  PeriphClkInit: pointer to an RCC_PeriphCLKInitTypeDef structure that
  *         contains the configuration information for the Extended Peripherals
  *         clocks(I2S, SAI, LTDC RTC and TIM).
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
  uint32_t timeout = 0;
  uint32_t tmpreg = 0;
    
  /* Check the parameters */
  assert_param(IS_RCC_PERIPHCLOCK(PeriphClkInit->PeriphClockSelection));
  
  /*----------------------- SAI/I2S Configuration (PLLI2S) -------------------------*/
  
  /*----------------------- Common configuration SAI/I2S ---------------------------*/
  /* In Case of SAI or I2S Clock Configuration through PLLI2S, PLLI2SN division   
     factor is common parameters for both peripherals */ 
  if((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2S) == RCC_PERIPHCLK_I2S) || 
     (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI_PLLI2S) == RCC_PERIPHCLK_SAI_PLLI2S))
  {
    /* check for Parameters */
    assert_param(IS_RCC_PLLI2SN_VALUE(PeriphClkInit->PLLI2S.PLLI2SN));
        
    /* Disable the PLLI2S */
    __HAL_RCC_PLLI2S_DISABLE();    
    /* Get new Timeout value */
    timeout = HAL_GetTick() + PLLI2S_TIMEOUT_VALUE;
    /* Wait till PLLI2S is disabled */
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLI2SRDY)  != RESET)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* return in case of Timeout detected */         
        return HAL_TIMEOUT;
      }
    }
    
    /*---------------------------- I2S configuration -------------------------------*/
    /* In Case of I2S Clock Configuration through PLLI2S, PLLI2SR must be added   
      only for I2S configuration */     
    if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2S) == (RCC_PERIPHCLK_I2S))
    {
      /* check for Parameters */
      assert_param(IS_RCC_PLLI2SR_VALUE(PeriphClkInit->PLLI2S.PLLI2SR));
      /* Configure the PLLI2S division factors */
      /* PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input) × (PLLI2SN/PLLM) */
      /* I2SCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SR */
      __HAL_RCC_PLLI2S_CONFIG(PeriphClkInit->PLLI2S.PLLI2SN , PeriphClkInit->PLLI2S.PLLI2SR);
    }
  
    /*---------------------------- SAI configuration -------------------------------*/ 
    /* In Case of SAI Clock Configuration through PLLI2S, PLLI2SQ and PLLI2S_DIVQ must  
       be added only for SAI configuration */     
    if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI_PLLI2S) == (RCC_PERIPHCLK_SAI_PLLI2S))
    {
      /* Check the PLLI2S division factors */
      assert_param(IS_RCC_PLLI2SQ_VALUE(PeriphClkInit->PLLI2S.PLLI2SQ));
      assert_param(IS_RCC_PLLI2S_DIVQ_VALUE(PeriphClkInit->PLLI2SDivQ));
      
      /* Read PLLI2SR value from PLLI2SCFGR register (this value is not need for SAI configuration) */
      tmpreg = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SR));
      /* Configure the PLLI2S division factors */      
      /* PLLI2S_VCO Input  = PLL_SOURCE/PLLM */
      /* PLLI2S_VCO Output = PLLI2S_VCO Input * PLLI2SN */
      /* SAI_CLK(first level) = PLLI2S_VCO Output/PLLI2SQ */
      __HAL_RCC_PLLI2S_SAICLK_CONFIG(PeriphClkInit->PLLI2S.PLLI2SN , PeriphClkInit->PLLI2S.PLLI2SQ , tmpreg);
      /* SAI_CLK_x = SAI_CLK(first level)/PLLI2SDIVQ */ 
      __HAL_RCC_PLLI2S_PLLSAICLKDIVQ_CONFIG(PeriphClkInit->PLLI2SDivQ);
    }
    
    /* Enable the PLLI2S */
    __HAL_RCC_PLLI2S_ENABLE();
    /* Get new Timeout value */
    timeout = HAL_GetTick() + PLLI2S_TIMEOUT_VALUE;
    /* Wait till PLLI2S is ready */
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLI2SRDY)  == RESET)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* return in case of Timeout detected */         
        return HAL_TIMEOUT;
      }
    }
  }
  
  /*----------------------- SAI/LTDC Configuration (PLLSAI) -------------------------*/
  
  /*----------------------- Common configuration SAI/LTDC ---------------------------*/ 
  /* In Case of SAI or LTDC Clock Configuration through PLLSAI, PLLSAIN division   
     factor is common parameters for both peripherals */ 
  if((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI_PLLSAI) == RCC_PERIPHCLK_SAI_PLLSAI) || 
     (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LTDC) == RCC_PERIPHCLK_LTDC))
  {
    /* Check the PLLSAI division factors */
    assert_param(IS_RCC_PLLSAIN_VALUE(PeriphClkInit->PLLSAI.PLLSAIN));
 
    /* Disable PLLSAI Clock */
    __HAL_RCC_PLLSAI_DISABLE(); 
    /* Get new Timeout value */    
    timeout = HAL_GetTick() + PLLSAI_TIMEOUT_VALUE;
    /* Wait till PLLSAI is disabled */
    while(__HAL_RCC_PLLSAI_GET_FLAG() != RESET)
    {
      if(HAL_GetTick() >= timeout)
      { 
        /* return in case of Timeout detected */        
        return HAL_TIMEOUT;
      }
    }
    
    /*---------------------------- SAI configuration -------------------------------*/
    /* In Case of SAI Clock Configuration through PLLSAI, PLLSAIQ and PLLSAI_DIVQ must  
       be added only for SAI configuration */     
    if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI_PLLSAI) == (RCC_PERIPHCLK_SAI_PLLSAI))
    {
      assert_param(IS_RCC_PLLSAIQ_VALUE(PeriphClkInit->PLLSAI.PLLSAIQ));
      assert_param(IS_RCC_PLLSAI_DIVQ_VALUE(PeriphClkInit->PLLSAIDivQ));
      
      /* Read PLLSAIR value from PLLSAICFGR register (this value is not need for SAI configuration) */
      tmpreg = ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIR) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIR));
      /* PLLSAI_VCO Input  = PLL_SOURCE/PLLM */
      /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN */
      /* SAI_CLK(first level) = PLLSAI_VCO Output/PLLSAIQ */
      __HAL_RCC_PLLSAI_CONFIG(PeriphClkInit->PLLSAI.PLLSAIN , PeriphClkInit->PLLSAI.PLLSAIQ, tmpreg);
      /* SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ */ 
      __HAL_RCC_PLLSAI_PLLSAICLKDIVQ_CONFIG(PeriphClkInit->PLLSAIDivQ);
    }
    
    /*---------------------------- LTDC configuration -------------------------------*/
    if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LTDC) == (RCC_PERIPHCLK_LTDC))
    {
      assert_param(IS_RCC_PLLSAIR_VALUE(PeriphClkInit->PLLSAI.PLLSAIR));
      assert_param(IS_RCC_PLLSAI_DIVR_VALUE(PeriphClkInit->PLLSAIDivR));
      
      /* Read PLLSAIR value from PLLSAICFGR register (this value is not need for SAI configuration) */
      tmpreg = ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIQ) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIQ));
      /* PLLSAI_VCO Input  = PLL_SOURCE/PLLM */
      /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN */
      /* LTDC_CLK(first level) = PLLSAI_VCO Output/PLLSAIR */
      __HAL_RCC_PLLSAI_CONFIG(PeriphClkInit->PLLSAI.PLLSAIN , tmpreg, PeriphClkInit->PLLSAI.PLLSAIR);
      /* LTDC_CLK = LTDC_CLK(first level)/PLLSAIDIVR */ 
      __HAL_RCC_PLLSAI_PLLSAICLKDIVR_CONFIG(PeriphClkInit->PLLSAIDivR);
    }    
    /* Enable PLLSAI Clock */
    __HAL_RCC_PLLSAI_ENABLE();
    /* Get new Timeout value */    
    timeout = HAL_GetTick() + PLLSAI_TIMEOUT_VALUE;
    /* Wait till PLLSAI is ready */
    while(__HAL_RCC_PLLSAI_GET_FLAG() == RESET)
    {
      if(HAL_GetTick() >= timeout)
      { 
        /* return in case of Timeout detected */        
        return HAL_TIMEOUT;
      }
    }  
  }

   
  /*---------------------------- RTC configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_RTC) == (RCC_PERIPHCLK_RTC))
  {
    /* Enable Power Clock*/
    __PWR_CLK_ENABLE();
    
    /* Enable write access to Backup domain */
    PWR->CR |= PWR_CR_DBP;
    
    /* Wait for Backup domain Write protection disable */
    timeout = HAL_GetTick() + DBP_TIMEOUT_VALUE;
    
    while((PWR->CR & PWR_CR_DBP) == RESET)
    {
      if(HAL_GetTick() >= timeout)
      {
        return HAL_TIMEOUT;
      }      
    }
    
    /* Reset the Backup domain only if the RTC Clock source selction is modified */ 
    if((RCC->BDCR & RCC_BDCR_RTCSEL) != (PeriphClkInit->RTCClockSelection & RCC_BDCR_RTCSEL))
    {
      /* Store the content of BDCR register before the reset of Backup Domain */
      tmpreg = (RCC->BDCR & ~(RCC_BDCR_RTCSEL));
      /* RTC Clock selection can be changed only if the Backup Domain is reset */
      __HAL_RCC_BACKUPRESET_FORCE();
      __HAL_RCC_BACKUPRESET_RELEASE();
      /* Restore the Content of BDCR register */
      RCC->BDCR = tmpreg;
    }
    
    /* If LSE is selected as RTC clock source, wait for LSE reactivation */
    if(PeriphClkInit->RTCClockSelection == RCC_RTCCLKSOURCE_LSE)
    {
      /* Get timeout */
      timeout = HAL_GetTick() + LSE_TIMEOUT_VALUE;
      
      /* Wait till LSE is ready */  
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)
      {
        if(HAL_GetTick() >= timeout)
        {
          return HAL_TIMEOUT;
        }      
      }  
    }
    __HAL_RCC_RTC_CONFIG(PeriphClkInit->RTCClockSelection); 
  }
  
  /*---------------------------- TIM configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_TIM) == (RCC_PERIPHCLK_TIM))
  {
    __HAL_RCC_TIMCLKPRESCALER(PeriphClkInit->TIMPresSelection);
  }
  return HAL_OK;
}

/**
  * @brief  Configures the RCC_OscInitStruct according to the internal 
  * RCC configuration registers.
  * @param  RCC_OscInitStruct: pointer to an RCC_OscInitTypeDef structure that 
  * will be configured.
  * @retval None
  */
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit)
{
  uint32_t tempreg;
  
  /* Set all possible values for the extended clock type parameter------------*/
  PeriphClkInit->PeriphClockSelection = RCC_PERIPHCLK_I2S | RCC_PERIPHCLK_SAI_PLLSAI | RCC_PERIPHCLK_SAI_PLLI2S | RCC_PERIPHCLK_LTDC | RCC_PERIPHCLK_TIM | RCC_PERIPHCLK_RTC;
  
  /* Get the PLLI2S Clock configuration -----------------------------------------------*/
  PeriphClkInit->PLLI2S.PLLI2SN = (uint32_t)((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SN));
  PeriphClkInit->PLLI2S.PLLI2SR = (uint32_t)((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SR));
  PeriphClkInit->PLLI2S.PLLI2SQ = (uint32_t)((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SQ) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SQ));
  /* Get the PLLSAI Clock configuration -----------------------------------------------*/
  PeriphClkInit->PLLSAI.PLLSAIN = (uint32_t)((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIN) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIN));
  PeriphClkInit->PLLSAI.PLLSAIR = (uint32_t)((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIR) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIR));
  PeriphClkInit->PLLSAI.PLLSAIQ = (uint32_t)((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIQ) >> POSITION_VAL(RCC_PLLSAICFGR_PLLSAIQ)); 
  /* Get the PLLSAI/PLLI2S division factors -----------------------------------------------*/
  PeriphClkInit->PLLI2SDivQ = (uint32_t)((RCC->DCKCFGR & RCC_DCKCFGR_PLLI2SDIVQ) >> POSITION_VAL(RCC_DCKCFGR_PLLI2SDIVQ));
  PeriphClkInit->PLLSAIDivQ = (uint32_t)((RCC->DCKCFGR & RCC_DCKCFGR_PLLSAIDIVQ) >> POSITION_VAL(RCC_DCKCFGR_PLLSAIDIVQ));
  PeriphClkInit->PLLSAIDivR = (uint32_t)(RCC->DCKCFGR & RCC_DCKCFGR_PLLSAIDIVR);
  /* Get the RTC Clock configuration -----------------------------------------------*/
  tempreg = (RCC->CFGR & RCC_CFGR_RTCPRE);
  PeriphClkInit->RTCClockSelection = (uint32_t)((tempreg) | (RCC->BDCR & RCC_BDCR_RTCSEL));
  
  if ((RCC->DCKCFGR & RCC_DCKCFGR_TIMPRE) == RESET)
  {
    PeriphClkInit->TIMPresSelection = RCC_TIMPRES_DESACTIVATED;
  }
  else
  {
    PeriphClkInit->TIMPresSelection = RCC_TIMPRES_ACTIVATED;
  }
}
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx)|| defined(STM32F417xx) || defined(STM32F401xC) || defined(STM32F401xE) 
/**
  * @brief  Initializes the RCC extended peripherals clocks according to the specified parameters in the
  *         RCC_PeriphCLKInitTypeDef.
  * @param  PeriphClkInit: pointer to an RCC_PeriphCLKInitTypeDef structure that
  *         contains the configuration information for the Extended Peripherals clocks(I2S and RTC clocks).
  *         
  * @note   A caution to be taken when HAL_RCCEx_PeriphCLKConfig() is used to select RTC clock selection, in this case 
  *         the Reset of Backup domain will be applied in order to modify the RTC Clock source as consequence all backup 
  *        domain (RTC and RCC_BDCR register expect BKPSRAM) will be reset
  *              
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit)
{
  uint32_t timeout = 0;
  uint32_t tmpreg = 0;
    
  /* Check the parameters */
  assert_param(IS_RCC_PERIPHCLOCK(PeriphClkInit->PeriphClockSelection));
  
  /*---------------------------- I2S configuration -------------------------------*/      
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2S) == (RCC_PERIPHCLK_I2S))
  {
    /* check for Parameters */
    assert_param(IS_RCC_PLLI2SR_VALUE(PeriphClkInit->PLLI2S.PLLI2SR));
    assert_param(IS_RCC_PLLI2SN_VALUE(PeriphClkInit->PLLI2S.PLLI2SN));
      
    /* Disable the PLLI2S */
    __HAL_RCC_PLLI2S_DISABLE();    
    /* Get new Timeout value */
    timeout = HAL_GetTick() + PLLI2S_TIMEOUT_VALUE;
    /* Wait till PLLI2S is disabled */
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLI2SRDY)  != RESET)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* return in case of Timeout detected */         
        return HAL_TIMEOUT;
      } 
    }
    /* Configure the PLLI2S division factors */
    /* PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input) × (PLLI2SN/PLLM) */
    /* I2SCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SR */
    __HAL_RCC_PLLI2S_CONFIG(PeriphClkInit->PLLI2S.PLLI2SN , PeriphClkInit->PLLI2S.PLLI2SR);
    
    /* Enable the PLLI2S */
    __HAL_RCC_PLLI2S_ENABLE();
    /* Get new Timeout value */
    timeout = HAL_GetTick() + PLLI2S_TIMEOUT_VALUE;
    /* Wait till PLLI2S is ready */
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLI2SRDY)  == RESET)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* return in case of Timeout detected */         
        return HAL_TIMEOUT;
      }
    }
  }
  
  /*---------------------------- RTC configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_RTC) == (RCC_PERIPHCLK_RTC))
  {
    /* Enable Power Clock*/
    __PWR_CLK_ENABLE();
    
    /* Enable write access to Backup domain */
    PWR->CR |= PWR_CR_DBP;
    
    /* Wait for Backup domain Write protection disable */
    timeout = HAL_GetTick() + DBP_TIMEOUT_VALUE;
    
    while((PWR->CR & PWR_CR_DBP) == RESET)
    {
      if(HAL_GetTick() >= timeout)
      {
        return HAL_TIMEOUT;
      }      
    }
        
    /* Reset the Backup domain only if the RTC Clock source selction is modified */ 
    if((RCC->BDCR & RCC_BDCR_RTCSEL) != (PeriphClkInit->RTCClockSelection & RCC_BDCR_RTCSEL))
    {
      /* Store the content of BDCR register before the reset of Backup Domain */
      tmpreg = (RCC->BDCR & ~(RCC_BDCR_RTCSEL));
      /* RTC Clock selection can be changed only if the Backup Domain is reset */
      __HAL_RCC_BACKUPRESET_FORCE();
      __HAL_RCC_BACKUPRESET_RELEASE();
      /* Restore the Content of BDCR register */
      RCC->BDCR = tmpreg;
    }
      
    /* If LSE is selected as RTC clock source, wait for LSE reactivation */
    if(PeriphClkInit->RTCClockSelection == RCC_RTCCLKSOURCE_LSE)
    {
      /* Get timeout */
      timeout = HAL_GetTick() + LSE_TIMEOUT_VALUE;
      
      /* Wait till LSE is ready */  
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)
      {
        if(HAL_GetTick() >= timeout)
        {
          return HAL_TIMEOUT;
        }      
      }  
    }
    __HAL_RCC_RTC_CONFIG(PeriphClkInit->RTCClockSelection); 
  }
  
  return HAL_OK;
}

/**
  * @brief  Configures the RCC_OscInitStruct according to the internal 
  * RCC configuration registers.
  * @param  RCC_OscInitStruct: pointer to an RCC_OscInitTypeDef structure that 
  * will be configured.
  * @retval None
  */
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit)
{
  uint32_t tempreg;
  
  /* Set all possible values for the extended clock type parameter------------*/
  PeriphClkInit->PeriphClockSelection = RCC_PERIPHCLK_I2S | RCC_PERIPHCLK_RTC;
  
  /* Get the PLLI2S Clock configuration -----------------------------------------------*/
  PeriphClkInit->PLLI2S.PLLI2SN = (uint32_t)((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SN));
  PeriphClkInit->PLLI2S.PLLI2SR = (uint32_t)((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SR));
  
  /* Get the RTC Clock configuration -----------------------------------------------*/
  tempreg = (RCC->CFGR & RCC_CFGR_RTCPRE);
  PeriphClkInit->RTCClockSelection = (uint32_t)((tempreg) | (RCC->BDCR & RCC_BDCR_RTCSEL));
  
}
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F401xC || STM32F401xE */
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
