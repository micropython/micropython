/**
  ******************************************************************************
  * @file    stm32n6xx_hal_rcc_ex.c
  * @author  MCD Application Team
  * @brief   Extended RCC HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities RCC extension peripheral:
  *           + Extended Peripheral Control functions
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
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup RCCEx  RCCEx
  * @brief RCC HAL module driver
  * @{
  */

#ifdef HAL_RCC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup RCCEx_Private_Functions_Prototypes RCCEx Private Functions Prototypes
  * @{
  */
static uint32_t RCCEx_GetHCLKFreq(uint32_t SYSCLK_Frequency);
static uint32_t RCCEx_GetPCLK1Freq(uint32_t HCLK_Frequency);
static uint32_t RCCEx_GetPCLK2Freq(uint32_t PCLK_Frequency);
static uint32_t RCCEx_GetPCLK4Freq(uint32_t PCLK_Frequency);
static uint32_t RCCEx_GetPCLK5Freq(uint32_t PCLK_Frequency);
static uint32_t RCCEx_GetPLLSourceFreq(uint32_t PLLsource);
static uint32_t RCCEx_CalcPLLFreq(uint32_t PLLInputFreq, uint32_t M, uint32_t N,
                                  uint32_t FRACN, uint32_t P1, uint32_t P2);
static uint32_t RCCEx_GetADCCLKFreq(uint32_t ADCxSource);
static uint32_t RCCEx_GetADFCLKFreq(uint32_t ADFxSource);
static uint32_t RCCEx_GetCLKPCLKFreq(uint32_t CLKPxSource);
static uint32_t RCCEx_GetCSICLKFreq(void);
static uint32_t RCCEx_GetDCMIPPCLKFreq(uint32_t DCMIPPxSource);
static uint32_t RCCEx_GetETH1CLKFreq(uint32_t ETH1xSource);
static uint32_t RCCEx_GetETH1PTPCLKFreq(uint32_t ETH1PTPxSource);
static uint32_t RCCEx_GetFDCANCLKFreq(uint32_t FDCANxSource);
static uint32_t RCCEx_GetFMCCLKFreq(uint32_t FMCxSource);
static uint32_t RCCEx_GetI2CCLKFreq(uint32_t I2CxSource);
static uint32_t RCCEx_GetI3CCLKFreq(uint32_t I3CxSource);
static uint32_t RCCEx_GetLPTIMCLKFreq(uint32_t LPTIMxSource);
static uint32_t RCCEx_GetLPUARTCLKFreq(uint32_t LPUARTxSource);
static uint32_t RCCEx_GetLTDCCLKFreq(uint32_t LPTIMxSource);
static uint32_t RCCEx_GetMDFCLKFreq(uint32_t MDFxSource);
static uint32_t RCCEx_GetPSSICLKFreq(uint32_t PSSIxSource);
static uint32_t RCCEx_GetRTCCLKFreq(void);
static uint32_t RCCEx_GetSAICLKFreq(uint32_t SAIxSource);
static uint32_t RCCEx_GetSDMMCCLKFreq(uint32_t SDMMCxSource);
static uint32_t RCCEx_GetSPDIFRXCLKFreq(uint32_t SPDIFRXxSource);
static uint32_t RCCEx_GetSPICLKFreq(uint32_t SPIxSource);
static uint32_t RCCEx_GetUARTCLKFreq(uint32_t UARTxSource);
static uint32_t RCCEx_GetUSARTCLKFreq(uint32_t USARTxSource);
static uint32_t RCCEx_GetOTGPHYCLKFreq(uint32_t OTGPHYxSource);
static uint32_t RCCEx_GetOTGPHYCKREFCLKFreq(uint32_t OTGPHYxCKREFSource);
static uint32_t RCCEx_GetXSPICLKFreq(uint32_t XSPIxSource);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
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
        the backup registers) and RCC_BDCR register are set to their reset values.

@endverbatim
  * @{
  */
/**
  * @brief  Initializes the RCC extended peripherals clocks according to the specified
  *         parameters in the RCC_PeriphCLKInitTypeDef.
  * @param  PeriphClkInit: pointer to an RCC_PeriphCLKInitTypeDef structure that
  *         contains the configuration information for the Extended Peripherals clocks
  *         (ADC, ADF1, CKPER, DCMI, DCMIPP, FDCAN, FMC, I2C1, I2C2, I2C3, I2C4, I3C1, I3C2,
  *         LPTIM1, LPTIM2, LPTIM3, LPTIM4, LPTIM5, LTDC, LPUART1, MDF1, PSSI, RTC, SAI1, SAI2,
  *         SDMMC1, SDMMC2, SPDIFRX1, SPI1, SPI2, SPI3, SPI4, SPI5, SPI6, TIM,
  *         USART1, USART2, USART3, UART4, UART5, USART6, UART7, UART8, UART9, USART10,
  *         USBPHY1, USBPHY2, USB OTGHS1, USB OTGHS2, XSPI1, XSPI2, XSPI3).
  * @note   Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
  *         the RTC clock source; in this case the Backup domain will be reset in
  *         order to modify the RTC Clock source, as consequence RTC registers (including
  *         the backup registers) are set to their reset values.
  * @note   Dynamic switches are used for peripheral input clock selection meaning the switch setting
  *         can only be changed if both input clocks are present during transition time.
  * @note   Care must be taken when RCC_PPPCLKSOURCE_ICx is selected. The ICx ClockSelection and ClockDivider fields
  *         modification indeed impacts all peripherals using this ICx as clock source.
  *
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(const RCC_PeriphCLKInitTypeDef  *PeriphClkInit)
{
  uint32_t tmpreg;
  uint32_t tickstart;
  HAL_StatusTypeDef ret = HAL_OK;      /* Intermediate status */
  HAL_StatusTypeDef status = HAL_OK;   /* Final status */

  /* Check the parameters */
  assert_param(IS_RCC_PERIPHCLOCK(PeriphClkInit->PeriphClockSelection));

  /*---------------------------- RTC configuration -------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_RTC) == RCC_PERIPHCLK_RTC)
  {
    /* check for RTC Parameters used to output RTCCLK */
    assert_param(IS_RCC_RTCCLKSOURCE(PeriphClkInit->RTCClockSelection));

    /* As the RTC clock source selection can be changed only if the Backup Domain is reset */
    /* reset the Backup domain only if the RTC Clock source selection is modified from default reset value */
    tmpreg = LL_RCC_GetRTCClockSource();

    if ((tmpreg != RCC_RTCCLKSOURCE_DISABLE) && (tmpreg != (PeriphClkInit->RTCClockSelection & RCC_CCIPR7_RTCSEL)))
    {
      /* Enable write access to Backup domain */
      SET_BIT(PWR->DBPCR, PWR_DBPCR_DBP);

      /* Read back to check Backup domain enabled */
      if (READ_BIT(PWR->DBPCR, PWR_DBPCR_DBP) == 0U)
      {
        ret = HAL_ERROR;
      }
      else
      {
#if 0 /* TO DO */
        /* Store the content of BDCR register before the reset of Backup Domain */
        /* excepted the RTC clock source selection that will be changed */
        tmpreg = READ_BIT(RCC->BDCR, ~(RCC_BDCR_RTCSEL));
        __HAL_RCC_BACKUPRESET_FORCE();
        __HAL_RCC_BACKUPRESET_RELEASE();
        /* Restore the content of BDCR register */
        WRITE_REG(RCC->BDCR, tmpreg);
#endif /* #if 0  TO DO */
      }
    }

    if (ret == HAL_OK)
    {
      /* If LSE is selected as RTC clock source (and enabled prior to Backup Domain reset), wait for LSE reactivation */
      if (PeriphClkInit->RTCClockSelection == RCC_RTCCLKSOURCE_LSE)
      {
        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till LSE is ready */
        while (LL_RCC_LSE_IsReady() == 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
          {
            ret = HAL_TIMEOUT;
            break;
          }
        }
      }

      if (ret == HAL_OK)
      {
        __HAL_RCC_RTC_CONFIG(PeriphClkInit->RTCClockSelection);
      }
      else
      {
        /* set overall return value */
        status = ret;
      }
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

  /*------------------------------------ CKPER configuration --------------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_CKPER) == RCC_PERIPHCLK_CKPER)
  {
    /* Check the parameters */
    assert_param(IS_RCC_CKPERCLKSOURCE(PeriphClkInit->CkperClockSelection));

    if (PeriphClkInit->CkperClockSelection == RCC_CLKPCLKSOURCE_IC5)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC5].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC5].ClockDivider));

      /* Set IC5 configuration */
      MODIFY_REG(RCC->IC5CFGR, RCC_IC5CFGR_IC5SEL | RCC_IC5CFGR_IC5INT,
                 PeriphClkInit->ICSelection[RCC_IC5].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC5].ClockDivider - 1U) << RCC_IC5CFGR_IC5INT_Pos));

      LL_RCC_IC5_Enable();
    }
    else if (PeriphClkInit->CkperClockSelection == RCC_CLKPCLKSOURCE_IC10)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC10].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC10].ClockDivider));

      /* Set IC10 configuration */
      MODIFY_REG(RCC->IC10CFGR, RCC_IC10CFGR_IC10SEL | RCC_IC10CFGR_IC10INT,
                 PeriphClkInit->ICSelection[RCC_IC10].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC10].ClockDivider - 1U) << RCC_IC10CFGR_IC10INT_Pos));

      LL_RCC_IC10_Enable();
    }
    else if (PeriphClkInit->CkperClockSelection == RCC_CLKPCLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->CkperClockSelection == RCC_CLKPCLKSOURCE_IC19)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC19].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC19].ClockDivider));

      /* Set IC19 configuration */
      MODIFY_REG(RCC->IC19CFGR, RCC_IC19CFGR_IC19SEL | RCC_IC19CFGR_IC19INT,
                 PeriphClkInit->ICSelection[RCC_IC19].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC19].ClockDivider - 1U) << RCC_IC19CFGR_IC19INT_Pos));

      LL_RCC_IC19_Enable();
    }
    else if (PeriphClkInit->CkperClockSelection == RCC_CLKPCLKSOURCE_IC20)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC20].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC20].ClockDivider));

      /* Set IC20 configuration */
      MODIFY_REG(RCC->IC20CFGR, RCC_IC20CFGR_IC20SEL | RCC_IC20CFGR_IC20INT,
                 PeriphClkInit->ICSelection[RCC_IC20].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC20].ClockDivider - 1U) << RCC_IC20CFGR_IC20INT_Pos));

      LL_RCC_IC20_Enable();
    }
    else
    {
      /* No ICx selected as source */
    }

    /* Configure the CKPER clock source */
    __HAL_RCC_CLKP_CONFIG(PeriphClkInit->CkperClockSelection);
  }

  /*-------------------------- XSPI1 clock source configuration ----------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_XSPI1) == RCC_PERIPHCLK_XSPI1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_XSPI1CLKSOURCE(PeriphClkInit->Xspi1ClockSelection));

    if (PeriphClkInit->Xspi1ClockSelection == RCC_XSPI1CLKSOURCE_IC3)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC3].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC3].ClockDivider));

      /* Set IC3 configuration */
      MODIFY_REG(RCC->IC3CFGR, RCC_IC3CFGR_IC3SEL | RCC_IC3CFGR_IC3INT,
                 PeriphClkInit->ICSelection[RCC_IC3].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC3].ClockDivider - 1U) << RCC_IC3CFGR_IC3INT_Pos));

      LL_RCC_IC3_Enable();
    }
    else if (PeriphClkInit->Xspi1ClockSelection == RCC_XSPI1CLKSOURCE_IC4)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC4].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC4].ClockDivider));

      /* Set IC4 configuration */
      MODIFY_REG(RCC->IC4CFGR, RCC_IC4CFGR_IC4SEL | RCC_IC4CFGR_IC4INT,
                 PeriphClkInit->ICSelection[RCC_IC4].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC4].ClockDivider - 1U) << RCC_IC4CFGR_IC4INT_Pos));

      LL_RCC_IC4_Enable();
    }
    else if (PeriphClkInit->Xspi1ClockSelection == RCC_XSPI1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Configure the XSPI1 clock source */
    __HAL_RCC_XSPI1_CONFIG(PeriphClkInit->Xspi1ClockSelection);
  }

  /*-------------------------- XSPI2 clock source configuration ----------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_XSPI2) == RCC_PERIPHCLK_XSPI2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_XSPI2CLKSOURCE(PeriphClkInit->Xspi2ClockSelection));

    if (PeriphClkInit->Xspi2ClockSelection == RCC_XSPI2CLKSOURCE_IC3)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC3].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC3].ClockDivider));

      /* Set IC3 configuration */
      MODIFY_REG(RCC->IC3CFGR, RCC_IC3CFGR_IC3SEL | RCC_IC3CFGR_IC3INT,
                 PeriphClkInit->ICSelection[RCC_IC3].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC3].ClockDivider - 1U) << RCC_IC3CFGR_IC3INT_Pos));

      LL_RCC_IC3_Enable();
    }
    else if (PeriphClkInit->Xspi2ClockSelection == RCC_XSPI2CLKSOURCE_IC4)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC4].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC4].ClockDivider));

      /* Set IC4 configuration */
      MODIFY_REG(RCC->IC4CFGR, RCC_IC4CFGR_IC4SEL | RCC_IC4CFGR_IC4INT,
                 PeriphClkInit->ICSelection[RCC_IC4].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC4].ClockDivider - 1U) << RCC_IC4CFGR_IC4INT_Pos));

      LL_RCC_IC4_Enable();
    }
    else if (PeriphClkInit->Xspi2ClockSelection == RCC_XSPI2CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Configure the XSPI2 clock source */
    __HAL_RCC_XSPI2_CONFIG(PeriphClkInit->Xspi2ClockSelection);
  }

  /*-------------------------- XSPI3 clock source configuration ----------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_XSPI3) == RCC_PERIPHCLK_XSPI3)
  {
    /* Check the parameters */
    assert_param(IS_RCC_XSPI3CLKSOURCE(PeriphClkInit->Xspi3ClockSelection));

    if (PeriphClkInit->Xspi3ClockSelection == RCC_XSPI3CLKSOURCE_IC3)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC3].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC3].ClockDivider));

      /* Set IC3 configuration */
      MODIFY_REG(RCC->IC3CFGR, RCC_IC3CFGR_IC3SEL | RCC_IC3CFGR_IC3INT,
                 PeriphClkInit->ICSelection[RCC_IC3].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC3].ClockDivider - 1U) << RCC_IC3CFGR_IC3INT_Pos));

      LL_RCC_IC3_Enable();
    }
    else if (PeriphClkInit->Xspi3ClockSelection == RCC_XSPI3CLKSOURCE_IC4)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC4].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC4].ClockDivider));

      /* Set IC4 configuration */
      MODIFY_REG(RCC->IC4CFGR, RCC_IC4CFGR_IC4SEL | RCC_IC4CFGR_IC4INT,
                 PeriphClkInit->ICSelection[RCC_IC4].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC4].ClockDivider - 1U) << RCC_IC4CFGR_IC4INT_Pos));

      LL_RCC_IC4_Enable();
    }
    else if (PeriphClkInit->Xspi3ClockSelection == RCC_XSPI3CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Configure the XSPI3 clock source */
    __HAL_RCC_XSPI3_CONFIG(PeriphClkInit->Xspi3ClockSelection);
  }

  /*---------------------------- FMC configuration -------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_FMC) == RCC_PERIPHCLK_FMC)
  {
    /* Check the parameters */
    assert_param(IS_RCC_FMCCLKSOURCE(PeriphClkInit->FmcClockSelection));

    if (PeriphClkInit->FmcClockSelection == RCC_FMCCLKSOURCE_IC3)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC3].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC3].ClockDivider));

      /* Set IC3 configuration */
      MODIFY_REG(RCC->IC3CFGR, RCC_IC3CFGR_IC3SEL | RCC_IC3CFGR_IC3INT,
                 PeriphClkInit->ICSelection[RCC_IC3].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC3].ClockDivider - 1U) << RCC_IC3CFGR_IC3INT_Pos));

      LL_RCC_IC3_Enable();
    }
    else if (PeriphClkInit->FmcClockSelection == RCC_FMCCLKSOURCE_IC4)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC4].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC4].ClockDivider));

      /* Set IC4 configuration */
      MODIFY_REG(RCC->IC4CFGR, RCC_IC4CFGR_IC4SEL | RCC_IC4CFGR_IC4INT,
                 PeriphClkInit->ICSelection[RCC_IC4].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC4].ClockDivider - 1U) << RCC_IC4CFGR_IC4INT_Pos));

      LL_RCC_IC4_Enable();
    }
    else if (PeriphClkInit->FmcClockSelection == RCC_FMCCLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of FMC kernel clock*/
    __HAL_RCC_FMC_CONFIG(PeriphClkInit->FmcClockSelection);
  }

  /*------------------------------------- SDMMC1 Configuration ------------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_SDMMC1) == RCC_PERIPHCLK_SDMMC1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SDMMC1CLKSOURCE(PeriphClkInit->Sdmmc1ClockSelection));

    if (PeriphClkInit->Sdmmc1ClockSelection == RCC_SDMMC1CLKSOURCE_IC4)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC4].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC4].ClockDivider));

      /* Set IC4 configuration */
      MODIFY_REG(RCC->IC4CFGR, RCC_IC4CFGR_IC4SEL | RCC_IC4CFGR_IC4INT,
                 PeriphClkInit->ICSelection[RCC_IC4].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC4].ClockDivider - 1U) << RCC_IC4CFGR_IC4INT_Pos));

      LL_RCC_IC4_Enable();
    }
    else if (PeriphClkInit->Sdmmc1ClockSelection == RCC_SDMMC1CLKSOURCE_IC5)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC5].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC5].ClockDivider));

      /* Set IC5 configuration */
      MODIFY_REG(RCC->IC5CFGR, RCC_IC5CFGR_IC5SEL | RCC_IC5CFGR_IC5INT,
                 PeriphClkInit->ICSelection[RCC_IC5].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC5].ClockDivider - 1U) << RCC_IC5CFGR_IC5INT_Pos));

      LL_RCC_IC5_Enable();
    }
    else if (PeriphClkInit->Sdmmc1ClockSelection == RCC_SDMMC1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of SDMMC1 clock*/
    __HAL_RCC_SDMMC1_CONFIG(PeriphClkInit->Sdmmc1ClockSelection);
  }

  /*------------------------------------- SDMMC2 Configuration ------------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_SDMMC2) == RCC_PERIPHCLK_SDMMC2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SDMMC2CLKSOURCE(PeriphClkInit->Sdmmc2ClockSelection));

    if (PeriphClkInit->Sdmmc2ClockSelection == RCC_SDMMC2CLKSOURCE_IC4)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC4].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC4].ClockDivider));

      /* Set IC4 configuration */
      MODIFY_REG(RCC->IC4CFGR, RCC_IC4CFGR_IC4SEL | RCC_IC4CFGR_IC4INT,
                 PeriphClkInit->ICSelection[RCC_IC4].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC4].ClockDivider - 1U) << RCC_IC4CFGR_IC4INT_Pos));

      LL_RCC_IC4_Enable();
    }
    else if (PeriphClkInit->Sdmmc2ClockSelection == RCC_SDMMC2CLKSOURCE_IC5)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC5].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC5].ClockDivider));

      /* Set IC5 configuration */
      MODIFY_REG(RCC->IC5CFGR, RCC_IC5CFGR_IC5SEL | RCC_IC5CFGR_IC5INT,
                 PeriphClkInit->ICSelection[RCC_IC5].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC5].ClockDivider - 1U) << RCC_IC5CFGR_IC5INT_Pos));

      LL_RCC_IC5_Enable();
    }
    else if (PeriphClkInit->Sdmmc2ClockSelection == RCC_SDMMC2CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of SDMMC2 clock*/
    __HAL_RCC_SDMMC2_CONFIG(PeriphClkInit->Sdmmc2ClockSelection);
  }

  /*---------------------------- ADC configuration -------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_ADC) == RCC_PERIPHCLK_ADC)
  {
    /* Check the parameters */
    assert_param(IS_RCC_ADCCLKSOURCE(PeriphClkInit->AdcClockSelection));
    assert_param(IS_RCC_ADCDIVIDER(PeriphClkInit->AdcDivider));

    if (PeriphClkInit->AdcClockSelection == RCC_ADCCLKSOURCE_IC7)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC7].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC7].ClockDivider));

      /* Set IC7 configuration */
      MODIFY_REG(RCC->IC7CFGR, RCC_IC7CFGR_IC7SEL | RCC_IC7CFGR_IC7INT,
                 PeriphClkInit->ICSelection[RCC_IC7].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC7].ClockDivider - 1U) << RCC_IC7CFGR_IC7INT_Pos));

      LL_RCC_IC7_Enable();
    }
    else if (PeriphClkInit->AdcClockSelection == RCC_ADCCLKSOURCE_IC8)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC8].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC8].ClockDivider));

      /* Set IC8 configuration */
      MODIFY_REG(RCC->IC8CFGR, RCC_IC8CFGR_IC8SEL | RCC_IC8CFGR_IC8INT,
                 PeriphClkInit->ICSelection[RCC_IC8].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC8].ClockDivider - 1U) << RCC_IC8CFGR_IC8INT_Pos));

      LL_RCC_IC8_Enable();
    }
    else if (PeriphClkInit->AdcClockSelection == RCC_ADCCLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Configure the ADC clock source and divider */
    MODIFY_REG(RCC->CCIPR1, (RCC_CCIPR1_ADCPRE | RCC_CCIPR1_ADC12SEL), \
               (((PeriphClkInit->AdcDivider - 1U) << RCC_CCIPR1_ADCPRE_Pos) | (PeriphClkInit->AdcClockSelection)));
  }

  /*---------------------------- ADF1 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_ADF1) == RCC_PERIPHCLK_ADF1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_ADF1CLKSOURCE(PeriphClkInit->Adf1ClockSelection));

    if (PeriphClkInit->Adf1ClockSelection == RCC_ADF1CLKSOURCE_IC7)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC7].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC7].ClockDivider));

      /* Set IC7 configuration */
      MODIFY_REG(RCC->IC7CFGR, RCC_IC7CFGR_IC7SEL | RCC_IC7CFGR_IC7INT,
                 PeriphClkInit->ICSelection[RCC_IC7].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC7].ClockDivider - 1U) << RCC_IC7CFGR_IC7INT_Pos));

      LL_RCC_IC7_Enable();
    }
    else if (PeriphClkInit->Adf1ClockSelection == RCC_ADF1CLKSOURCE_IC8)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC8].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC8].ClockDivider));

      /* Set IC8 configuration */
      MODIFY_REG(RCC->IC8CFGR, RCC_IC8CFGR_IC8SEL | RCC_IC8CFGR_IC8INT,
                 PeriphClkInit->ICSelection[RCC_IC8].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC8].ClockDivider - 1U) << RCC_IC8CFGR_IC8INT_Pos));

      LL_RCC_IC8_Enable();
    }
    else if (PeriphClkInit->Adf1ClockSelection == RCC_ADF1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of ADF1 clock*/
    __HAL_RCC_ADF1_CONFIG(PeriphClkInit->Adf1ClockSelection);
  }

  /*------------------------------------ CSI configuration --------------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_CSI) == RCC_PERIPHCLK_CSI)
  {
    /* Check the parameters */
    assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC18].ClockSelection));
    assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC18].ClockDivider));

    /* Set IC18 configuration */
    MODIFY_REG(RCC->IC18CFGR, RCC_IC18CFGR_IC18SEL | RCC_IC18CFGR_IC18INT,
               PeriphClkInit->ICSelection[RCC_IC18].ClockSelection | \
               ((PeriphClkInit->ICSelection[RCC_IC18].ClockDivider - 1U) << RCC_IC18CFGR_IC18INT_Pos));

    LL_RCC_IC18_Enable();
  }

  /*---------------------- DCMIPP configuration ------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_DCMIPP) == RCC_PERIPHCLK_DCMIPP)
  {
    /* Check the parameters */
    assert_param(IS_RCC_DCMIPPCLKSOURCE(PeriphClkInit->DcmippClockSelection));

    if (PeriphClkInit->DcmippClockSelection == RCC_DCMIPPCLKSOURCE_IC17)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC17].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC17].ClockDivider));

      /* Set IC17 configuration */
      MODIFY_REG(RCC->IC17CFGR, RCC_IC17CFGR_IC17SEL | RCC_IC17CFGR_IC17INT,
                 PeriphClkInit->ICSelection[RCC_IC17].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC17].ClockDivider - 1U) << RCC_IC17CFGR_IC17INT_Pos));

      LL_RCC_IC17_Enable();
    }
    else if (PeriphClkInit->DcmippClockSelection == RCC_DCMIPPCLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Configure the CEC clock source */
    __HAL_RCC_DCMIPP_CONFIG(PeriphClkInit->DcmippClockSelection);
  }

  /*---------------------- ETH1 configuration --------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_ETH1) == RCC_PERIPHCLK_ETH1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_ETH1CLKSOURCE(PeriphClkInit->Eth1ClockSelection));

    if (PeriphClkInit->Eth1ClockSelection == RCC_ETH1CLKSOURCE_IC12)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC12].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC12].ClockDivider));

      /* Set IC12 configuration */
      MODIFY_REG(RCC->IC12CFGR, RCC_IC12CFGR_IC12SEL | RCC_IC12CFGR_IC12INT,
                 PeriphClkInit->ICSelection[RCC_IC12].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC12].ClockDivider - 1U) << RCC_IC12CFGR_IC12INT_Pos));

      LL_RCC_IC12_Enable();
    }
    else if (PeriphClkInit->Eth1ClockSelection == RCC_ETH1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Configure the ETH1 clock source */
    __HAL_RCC_ETH1_CONFIG(PeriphClkInit->Eth1ClockSelection);
  }

  /*---------------------- ETH1PHY configuration -----------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_ETH1PHY) == RCC_PERIPHCLK_ETH1PHY)
  {
    /* Check the parameters */
    assert_param(IS_RCC_ETH1PHYIF(PeriphClkInit->Eth1PhyInterfaceSelection));

    /* Configure the source of ETH1 PHY interface */
    __HAL_RCC_ETH1PHY_CONFIG(PeriphClkInit->Eth1PhyInterfaceSelection);
  }

  /*---------------------- ETH1 RX configuration -----------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_ETH1RX) == RCC_PERIPHCLK_ETH1RX)
  {
    /* Check the parameters */
    assert_param(IS_RCC_ETH1RXCLKSOURCE(PeriphClkInit->Eth1RxClockSelection));

    /* Configure the ETH1 RX clock source */
    __HAL_RCC_ETH1RX_CONFIG(PeriphClkInit->Eth1RxClockSelection);
  }

  /*---------------------- ETH1 TX configuration -----------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_ETH1TX) == RCC_PERIPHCLK_ETH1TX)
  {
    /* Check the parameters */
    assert_param(IS_RCC_ETH1TXCLKSOURCE(PeriphClkInit->Eth1TxClockSelection));

    /* Configure the ETH1 TX clock source */
    __HAL_RCC_ETH1TX_CONFIG(PeriphClkInit->Eth1TxClockSelection);
  }

  /*---------------------- ETH1 PTP configuration ----------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_ETH1PTP) == RCC_PERIPHCLK_ETH1PTP)
  {
    /* Check the parameters */
    assert_param(IS_RCC_ETH1PTPCLKSOURCE(PeriphClkInit->Eth1PtpClockSelection));
    assert_param(IS_RCC_ETH1PTPDIVIDER(PeriphClkInit->Eth1PtpDivider));

    if (PeriphClkInit->Eth1PtpClockSelection == RCC_ETH1PTPCLKSOURCE_IC13)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC13].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC13].ClockDivider));

      /* Set IC13 configuration */
      MODIFY_REG(RCC->IC13CFGR, RCC_IC13CFGR_IC13SEL | RCC_IC13CFGR_IC13INT,
                 PeriphClkInit->ICSelection[RCC_IC13].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC13].ClockDivider - 1U) << RCC_IC13CFGR_IC13INT_Pos));

      LL_RCC_IC13_Enable();
    }
    else if (PeriphClkInit->Eth1PtpClockSelection == RCC_ETH1PTPCLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Configure the ETH1 PTP clock source and divider */
    MODIFY_REG(RCC->CCIPR2, (RCC_CCIPR2_ETH1PTPDIV | RCC_CCIPR2_ETH1PTPSEL), \
               (((PeriphClkInit->Eth1PtpDivider - 1U) << RCC_CCIPR2_ETH1PTPDIV_Pos) | PeriphClkInit->Eth1PtpClockSelection));
  }

  /*---------------------- FDCAN configuration -------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_FDCAN) == RCC_PERIPHCLK_FDCAN)
  {
    /* Check the parameters */
    assert_param(IS_RCC_FDCANCLKSOURCE(PeriphClkInit->FdcanClockSelection));

    if (PeriphClkInit->FdcanClockSelection == RCC_FDCANCLKSOURCE_IC19)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC19].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC19].ClockDivider));

      /* Set IC19 configuration */
      MODIFY_REG(RCC->IC19CFGR, RCC_IC19CFGR_IC19SEL | RCC_IC19CFGR_IC19INT,
                 PeriphClkInit->ICSelection[RCC_IC19].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC19].ClockDivider - 1U) << RCC_IC19CFGR_IC19INT_Pos));

      LL_RCC_IC19_Enable();
    }
    else if (PeriphClkInit->FdcanClockSelection == RCC_FDCANCLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of FDCAN clock*/
    __HAL_RCC_FDCAN_CONFIG(PeriphClkInit->FdcanClockSelection);
  }

  /*------------------------------ I2C1 Configuration ------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_I2C1) == RCC_PERIPHCLK_I2C1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_I2C1CLKSOURCE(PeriphClkInit->I2c1ClockSelection));

    if (PeriphClkInit->I2c1ClockSelection == RCC_I2C1CLKSOURCE_IC10)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC10].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC10].ClockDivider));

      /* Set IC10 configuration */
      MODIFY_REG(RCC->IC10CFGR, RCC_IC10CFGR_IC10SEL | RCC_IC10CFGR_IC10INT,
                 PeriphClkInit->ICSelection[RCC_IC10].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC10].ClockDivider - 1U) << RCC_IC10CFGR_IC10INT_Pos));

      LL_RCC_IC10_Enable();
    }
    else if (PeriphClkInit->I2c1ClockSelection == RCC_I2C1CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->I2c1ClockSelection == RCC_I2C1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of I2C1 clock*/
    __HAL_RCC_I2C1_CONFIG(PeriphClkInit->I2c1ClockSelection);
  }

  /*------------------------------ I2C2 Configuration ------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_I2C2) == RCC_PERIPHCLK_I2C2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_I2C2CLKSOURCE(PeriphClkInit->I2c2ClockSelection));

    if (PeriphClkInit->I2c2ClockSelection == RCC_I2C2CLKSOURCE_IC10)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC10].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC10].ClockDivider));

      /* Set IC10 configuration */
      MODIFY_REG(RCC->IC10CFGR, RCC_IC10CFGR_IC10SEL | RCC_IC10CFGR_IC10INT,
                 PeriphClkInit->ICSelection[RCC_IC10].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC10].ClockDivider - 1U) << RCC_IC10CFGR_IC10INT_Pos));

      LL_RCC_IC10_Enable();
    }
    else if (PeriphClkInit->I2c2ClockSelection == RCC_I2C2CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->I2c2ClockSelection == RCC_I2C2CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of I2C2 clock*/
    __HAL_RCC_I2C2_CONFIG(PeriphClkInit->I2c2ClockSelection);
  }

  /*------------------------------ I2C3 Configuration ------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_I2C3) == RCC_PERIPHCLK_I2C3)
  {
    /* Check the parameters */
    assert_param(IS_RCC_I2C3CLKSOURCE(PeriphClkInit->I2c3ClockSelection));

    if (PeriphClkInit->I2c3ClockSelection == RCC_I2C3CLKSOURCE_IC10)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC10].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC10].ClockDivider));

      /* Set IC10 configuration */
      MODIFY_REG(RCC->IC10CFGR, RCC_IC10CFGR_IC10SEL | RCC_IC10CFGR_IC10INT,
                 PeriphClkInit->ICSelection[RCC_IC10].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC10].ClockDivider - 1U) << RCC_IC10CFGR_IC10INT_Pos));

      LL_RCC_IC10_Enable();
    }
    else if (PeriphClkInit->I2c3ClockSelection == RCC_I2C3CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->I2c3ClockSelection == RCC_I2C3CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of I2C3 clock*/
    __HAL_RCC_I2C3_CONFIG(PeriphClkInit->I2c3ClockSelection);
  }

  /*------------------------------ I2C4 Configuration ------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_I2C4) == RCC_PERIPHCLK_I2C4)
  {
    /* Check the parameters */
    assert_param(IS_RCC_I2C4CLKSOURCE(PeriphClkInit->I2c4ClockSelection));

    if (PeriphClkInit->I2c4ClockSelection == RCC_I2C4CLKSOURCE_IC10)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC10].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC10].ClockDivider));

      /* Set IC10 configuration */
      MODIFY_REG(RCC->IC10CFGR, RCC_IC10CFGR_IC10SEL | RCC_IC10CFGR_IC10INT,
                 PeriphClkInit->ICSelection[RCC_IC10].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC10].ClockDivider - 1U) << RCC_IC10CFGR_IC10INT_Pos));

      LL_RCC_IC10_Enable();
    }
    else if (PeriphClkInit->I2c4ClockSelection == RCC_I2C4CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->I2c4ClockSelection == RCC_I2C4CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of I2C4 clock*/
    __HAL_RCC_I2C4_CONFIG(PeriphClkInit->I2c4ClockSelection);
  }

  /*------------------------------ I3C1 Configuration ------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_I3C1) == RCC_PERIPHCLK_I3C1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_I3C1CLKSOURCE(PeriphClkInit->I3c1ClockSelection));

    if (PeriphClkInit->I3c1ClockSelection == RCC_I3C1CLKSOURCE_IC10)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC10].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC10].ClockDivider));

      /* Set IC10 configuration */
      MODIFY_REG(RCC->IC10CFGR, RCC_IC10CFGR_IC10SEL | RCC_IC10CFGR_IC10INT,
                 PeriphClkInit->ICSelection[RCC_IC10].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC10].ClockDivider - 1U) << RCC_IC10CFGR_IC10INT_Pos));

      LL_RCC_IC10_Enable();
    }
    else if (PeriphClkInit->I3c1ClockSelection == RCC_I3C1CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->I3c1ClockSelection == RCC_I3C1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of I3C1 clock*/
    __HAL_RCC_I3C1_CONFIG(PeriphClkInit->I3c1ClockSelection);
  }

  /*------------------------------ I3C2 Configuration ------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_I3C2) == RCC_PERIPHCLK_I3C2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_I3C2CLKSOURCE(PeriphClkInit->I3c2ClockSelection));

    if (PeriphClkInit->I3c2ClockSelection == RCC_I3C2CLKSOURCE_IC10)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC10].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC10].ClockDivider));

      /* Set IC10 configuration */
      MODIFY_REG(RCC->IC10CFGR, RCC_IC10CFGR_IC10SEL | RCC_IC10CFGR_IC10INT,
                 PeriphClkInit->ICSelection[RCC_IC10].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC10].ClockDivider - 1U) << RCC_IC10CFGR_IC10INT_Pos));

      LL_RCC_IC10_Enable();
    }
    else if (PeriphClkInit->I3c2ClockSelection == RCC_I3C2CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->I3c2ClockSelection == RCC_I3C2CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of I3C2 clock*/
    __HAL_RCC_I3C2_CONFIG(PeriphClkInit->I3c2ClockSelection);
  }

  /*---------------------------- LPTIM1 configuration ------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_LPTIM1) == RCC_PERIPHCLK_LPTIM1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LPTIM1CLKSOURCE(PeriphClkInit->Lptim1ClockSelection));

    if (PeriphClkInit->Lptim1ClockSelection == RCC_LPTIM1CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->Lptim1ClockSelection == RCC_LPTIM1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of LPTIM1 clock*/
    __HAL_RCC_LPTIM1_CONFIG(PeriphClkInit->Lptim1ClockSelection);
  }

  /*---------------------------- LPTIM2 configuration ------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_LPTIM2) == RCC_PERIPHCLK_LPTIM2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LPTIM2CLKSOURCE(PeriphClkInit->Lptim2ClockSelection));

    if (PeriphClkInit->Lptim2ClockSelection == RCC_LPTIM2CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->Lptim2ClockSelection == RCC_LPTIM2CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of LPTIM2 clock*/
    __HAL_RCC_LPTIM2_CONFIG(PeriphClkInit->Lptim2ClockSelection);
  }

  /*---------------------------- LPTIM3 configuration -----------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_LPTIM3) == RCC_PERIPHCLK_LPTIM3)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LPTIM3CLKSOURCE(PeriphClkInit->Lptim3ClockSelection));

    if (PeriphClkInit->Lptim3ClockSelection == RCC_LPTIM3CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->Lptim3ClockSelection == RCC_LPTIM3CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of LPTIM3 clock */
    __HAL_RCC_LPTIM3_CONFIG(PeriphClkInit->Lptim3ClockSelection);
  }

  /*---------------------------- LPTIM4 configuration -----------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_LPTIM4) == RCC_PERIPHCLK_LPTIM4)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LPTIM4CLKSOURCE(PeriphClkInit->Lptim4ClockSelection));

    if (PeriphClkInit->Lptim4ClockSelection == RCC_LPTIM4CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->Lptim4ClockSelection == RCC_LPTIM4CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of LPTIM4 clock */
    __HAL_RCC_LPTIM4_CONFIG(PeriphClkInit->Lptim4ClockSelection);
  }

  /*---------------------------- LPTIM5 configuration -----------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_LPTIM5) == RCC_PERIPHCLK_LPTIM5)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LPTIM5CLKSOURCE(PeriphClkInit->Lptim5ClockSelection));

    if (PeriphClkInit->Lptim5ClockSelection == RCC_LPTIM5CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->Lptim5ClockSelection == RCC_LPTIM5CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of LPTIM5 clock */
    __HAL_RCC_LPTIM5_CONFIG(PeriphClkInit->Lptim5ClockSelection);
  }

  /*-------------------------- LPUART1 Configuration -------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_LPUART1) == RCC_PERIPHCLK_LPUART1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LPUART1CLKSOURCE(PeriphClkInit->Lpuart1ClockSelection));

    if (PeriphClkInit->Lpuart1ClockSelection == RCC_LPUART1CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Lpuart1ClockSelection == RCC_LPUART1CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Lpuart1ClockSelection == RCC_LPUART1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of LPUART1 clock */
    __HAL_RCC_LPUART1_CONFIG(PeriphClkInit->Lpuart1ClockSelection);
  }

  /*-------------------------- LTDC Configuration ----------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_LTDC) == RCC_PERIPHCLK_LTDC)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LTDCCLKSOURCE(PeriphClkInit->LtdcClockSelection));

    if (PeriphClkInit->LtdcClockSelection == RCC_LTDCCLKSOURCE_IC16)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC16].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC16].ClockDivider));

      /* Set IC16 configuration */
      MODIFY_REG(RCC->IC16CFGR, RCC_IC16CFGR_IC16SEL | RCC_IC16CFGR_IC16INT,
                 PeriphClkInit->ICSelection[RCC_IC16].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC16].ClockDivider - 1U) << RCC_IC16CFGR_IC16INT_Pos));

      LL_RCC_IC16_Enable();
    }
    else if (PeriphClkInit->LtdcClockSelection == RCC_LTDCCLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of LTDC clock */
    __HAL_RCC_LTDC_CONFIG(PeriphClkInit->LtdcClockSelection);
  }

  /*---------------------------- MDF1 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_MDF1) == RCC_PERIPHCLK_MDF1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_MDF1CLKSOURCE(PeriphClkInit->Mdf1ClockSelection));

    if (PeriphClkInit->Mdf1ClockSelection == RCC_MDF1CLKSOURCE_IC7)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC7].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC7].ClockDivider));

      /* Set IC7 configuration */
      MODIFY_REG(RCC->IC7CFGR, RCC_IC7CFGR_IC7SEL | RCC_IC7CFGR_IC7INT,
                 PeriphClkInit->ICSelection[RCC_IC7].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC7].ClockDivider - 1U) << RCC_IC7CFGR_IC7INT_Pos));

      LL_RCC_IC7_Enable();
    }
    else if (PeriphClkInit->Mdf1ClockSelection == RCC_MDF1CLKSOURCE_IC8)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC8].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC8].ClockDivider));

      /* Set IC8 configuration */
      MODIFY_REG(RCC->IC8CFGR, RCC_IC8CFGR_IC8SEL | RCC_IC8CFGR_IC8INT,
                 PeriphClkInit->ICSelection[RCC_IC8].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC8].ClockDivider - 1U) << RCC_IC8CFGR_IC8INT_Pos));

      LL_RCC_IC8_Enable();
    }
    else if (PeriphClkInit->Mdf1ClockSelection == RCC_MDF1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of MDF1 clock*/
    __HAL_RCC_MDF1_CONFIG(PeriphClkInit->Mdf1ClockSelection);
  }

  /*---------------------------- PSSI configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_PSSI) == RCC_PERIPHCLK_PSSI)
  {
    /* Check the parameters */
    assert_param(IS_RCC_PSSICLKSOURCE(PeriphClkInit->PssiClockSelection));

    if (PeriphClkInit->PssiClockSelection == RCC_PSSICLKSOURCE_IC20)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC20].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC20].ClockDivider));

      /* Set IC20 configuration */
      MODIFY_REG(RCC->IC20CFGR, RCC_IC20CFGR_IC20SEL | RCC_IC20CFGR_IC20INT,
                 PeriphClkInit->ICSelection[RCC_IC20].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC20].ClockDivider - 1U) << RCC_IC20CFGR_IC20INT_Pos));

      LL_RCC_IC20_Enable();
    }
    else if (PeriphClkInit->PssiClockSelection == RCC_PSSICLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of PSSI clock*/
    __HAL_RCC_PSSI_CONFIG(PeriphClkInit->PssiClockSelection);
  }

  /*---------------------------- SAI1 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_SAI1) == RCC_PERIPHCLK_SAI1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SAI1CLKSOURCE(PeriphClkInit->Sai1ClockSelection));

    if (PeriphClkInit->Sai1ClockSelection == RCC_SAI1CLKSOURCE_IC7)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC7].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC7].ClockDivider));

      /* Set IC7 configuration */
      MODIFY_REG(RCC->IC7CFGR, RCC_IC7CFGR_IC7SEL | RCC_IC7CFGR_IC7INT,
                 PeriphClkInit->ICSelection[RCC_IC7].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC7].ClockDivider - 1U) << RCC_IC7CFGR_IC7INT_Pos));

      LL_RCC_IC7_Enable();
    }
    else if (PeriphClkInit->Sai1ClockSelection == RCC_SAI1CLKSOURCE_IC8)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC8].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC8].ClockDivider));

      /* Set IC8 configuration */
      MODIFY_REG(RCC->IC8CFGR, RCC_IC8CFGR_IC8SEL | RCC_IC8CFGR_IC8INT,
                 PeriphClkInit->ICSelection[RCC_IC8].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC8].ClockDivider - 1U) << RCC_IC8CFGR_IC8INT_Pos));

      LL_RCC_IC8_Enable();
    }
    else if (PeriphClkInit->Sai1ClockSelection == RCC_SAI1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of SAI1 clock*/
    __HAL_RCC_SAI1_CONFIG(PeriphClkInit->Sai1ClockSelection);
  }

  /*---------------------------- SAI2 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_SAI2) == RCC_PERIPHCLK_SAI2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SAI2CLKSOURCE(PeriphClkInit->Sai2ClockSelection));

    if (PeriphClkInit->Sai2ClockSelection == RCC_SAI2CLKSOURCE_IC7)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC7].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC7].ClockDivider));

      /* Set IC7 configuration */
      MODIFY_REG(RCC->IC7CFGR, RCC_IC7CFGR_IC7SEL | RCC_IC7CFGR_IC7INT,
                 PeriphClkInit->ICSelection[RCC_IC7].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC7].ClockDivider - 1U) << RCC_IC7CFGR_IC7INT_Pos));

      LL_RCC_IC7_Enable();
    }
    else if (PeriphClkInit->Sai2ClockSelection == RCC_SAI2CLKSOURCE_IC8)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC8].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC8].ClockDivider));

      /* Set IC8 configuration */
      MODIFY_REG(RCC->IC8CFGR, RCC_IC8CFGR_IC8SEL | RCC_IC8CFGR_IC8INT,
                 PeriphClkInit->ICSelection[RCC_IC8].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC8].ClockDivider - 1U) << RCC_IC8CFGR_IC8INT_Pos));

      LL_RCC_IC8_Enable();
    }
    else if (PeriphClkInit->Sai2ClockSelection == RCC_SAI2CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of SAI2 clock*/
    __HAL_RCC_SAI2_CONFIG(PeriphClkInit->Sai2ClockSelection);
  }

  /*---------------------------- SPDIFRX1 configuration -------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_SPDIFRX1) == RCC_PERIPHCLK_SPDIFRX1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SPDIFRX1CLKSOURCE(PeriphClkInit->Spdifrx1ClockSelection));

    if (PeriphClkInit->Spdifrx1ClockSelection == RCC_SPDIFRX1CLKSOURCE_IC7)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC7].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC7].ClockDivider));

      /* Set IC7 configuration */
      MODIFY_REG(RCC->IC7CFGR, RCC_IC7CFGR_IC7SEL | RCC_IC7CFGR_IC7INT,
                 PeriphClkInit->ICSelection[RCC_IC7].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC7].ClockDivider - 1U) << RCC_IC7CFGR_IC7INT_Pos));

      LL_RCC_IC7_Enable();
    }
    else if (PeriphClkInit->Spdifrx1ClockSelection == RCC_SPDIFRX1CLKSOURCE_IC8)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC8].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC8].ClockDivider));

      /* Set IC8 configuration */
      MODIFY_REG(RCC->IC8CFGR, RCC_IC8CFGR_IC8SEL | RCC_IC8CFGR_IC8INT,
                 PeriphClkInit->ICSelection[RCC_IC8].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC8].ClockDivider - 1U) << RCC_IC8CFGR_IC8INT_Pos));

      LL_RCC_IC8_Enable();
    }
    else if (PeriphClkInit->Spdifrx1ClockSelection == RCC_SPDIFRX1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of SPDIFRX1 clock */
    __HAL_RCC_SPDIFRX1_CONFIG(PeriphClkInit->Spdifrx1ClockSelection);
  }

  /*---------------------------- SPI1 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_SPI1) == RCC_PERIPHCLK_SPI1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SPI1CLKSOURCE(PeriphClkInit->Spi1ClockSelection));

    if (PeriphClkInit->Spi1ClockSelection == RCC_SPI1CLKSOURCE_IC8)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC8].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC8].ClockDivider));

      /* Set IC8 configuration */
      MODIFY_REG(RCC->IC8CFGR, RCC_IC8CFGR_IC8SEL | RCC_IC8CFGR_IC8INT,
                 PeriphClkInit->ICSelection[RCC_IC8].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC8].ClockDivider - 1U) << RCC_IC8CFGR_IC8INT_Pos));

      LL_RCC_IC8_Enable();
    }
    else if (PeriphClkInit->Spi1ClockSelection == RCC_SPI1CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Spi1ClockSelection == RCC_SPI1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of SPI1 clock*/
    __HAL_RCC_SPI1_CONFIG(PeriphClkInit->Spi1ClockSelection);
  }

  /*---------------------------- SPI2 configuration ---------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_SPI2) == RCC_PERIPHCLK_SPI2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SPI2CLKSOURCE(PeriphClkInit->Spi2ClockSelection));

    if (PeriphClkInit->Spi2ClockSelection == RCC_SPI2CLKSOURCE_IC8)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC8].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC8].ClockDivider));

      /* Set IC8 configuration */
      MODIFY_REG(RCC->IC8CFGR, RCC_IC8CFGR_IC8SEL | RCC_IC8CFGR_IC8INT,
                 PeriphClkInit->ICSelection[RCC_IC8].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC8].ClockDivider - 1U) << RCC_IC8CFGR_IC8INT_Pos));

      LL_RCC_IC8_Enable();
    }
    else if (PeriphClkInit->Spi2ClockSelection == RCC_SPI2CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Spi2ClockSelection == RCC_SPI2CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of SPI2 clock*/
    __HAL_RCC_SPI2_CONFIG(PeriphClkInit->Spi2ClockSelection);
  }

  /*---------------------------- SPI3 configuration ---------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_SPI3) == RCC_PERIPHCLK_SPI3)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SPI3CLKSOURCE(PeriphClkInit->Spi3ClockSelection));

    if (PeriphClkInit->Spi3ClockSelection == RCC_SPI3CLKSOURCE_IC8)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC8].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC8].ClockDivider));

      /* Set IC8 configuration */
      MODIFY_REG(RCC->IC8CFGR, RCC_IC8CFGR_IC8SEL | RCC_IC8CFGR_IC8INT,
                 PeriphClkInit->ICSelection[RCC_IC8].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC8].ClockDivider - 1U) << RCC_IC8CFGR_IC8INT_Pos));

      LL_RCC_IC8_Enable();
    }
    else if (PeriphClkInit->Spi3ClockSelection == RCC_SPI3CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Spi3ClockSelection == RCC_SPI3CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of SPI3 clock*/
    __HAL_RCC_SPI3_CONFIG(PeriphClkInit->Spi3ClockSelection);
  }

  /*---------------------------- SPI4 configuration -------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_SPI4) == RCC_PERIPHCLK_SPI4)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SPI4CLKSOURCE(PeriphClkInit->Spi4ClockSelection));

    if (PeriphClkInit->Spi4ClockSelection == RCC_SPI4CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Spi4ClockSelection == RCC_SPI4CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Spi4ClockSelection == RCC_SPI4CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of SPI4 clock */
    __HAL_RCC_SPI4_CONFIG(PeriphClkInit->Spi4ClockSelection);
  }

  /*---------------------------- SPI5 configuration -------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_SPI5) == RCC_PERIPHCLK_SPI5)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SPI5CLKSOURCE(PeriphClkInit->Spi5ClockSelection));

    if (PeriphClkInit->Spi5ClockSelection == RCC_SPI5CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Spi5ClockSelection == RCC_SPI5CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Spi5ClockSelection == RCC_SPI5CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of SPI5 clock */
    __HAL_RCC_SPI5_CONFIG(PeriphClkInit->Spi5ClockSelection);
  }

  /*---------------------------- SPI6 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_SPI6) == RCC_PERIPHCLK_SPI6)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SPI6CLKSOURCE(PeriphClkInit->Spi6ClockSelection));

    if (PeriphClkInit->Spi6ClockSelection == RCC_SPI6CLKSOURCE_IC8)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC8].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC8].ClockDivider));

      /* Set IC8 configuration */
      MODIFY_REG(RCC->IC8CFGR, RCC_IC8CFGR_IC8SEL | RCC_IC8CFGR_IC8INT,
                 PeriphClkInit->ICSelection[RCC_IC8].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC8].ClockDivider - 1U) << RCC_IC8CFGR_IC8INT_Pos));

      LL_RCC_IC8_Enable();
    }
    else if (PeriphClkInit->Spi6ClockSelection == RCC_SPI6CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Spi6ClockSelection == RCC_SPI6CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of SPI6 clock*/
    __HAL_RCC_SPI6_CONFIG(PeriphClkInit->Spi6ClockSelection);
  }

  /*-------------------------- USART1 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_USART1) == RCC_PERIPHCLK_USART1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USART1CLKSOURCE(PeriphClkInit->Usart1ClockSelection));

    if (PeriphClkInit->Usart1ClockSelection == RCC_USART1CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Usart1ClockSelection == RCC_USART1CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Usart1ClockSelection == RCC_USART1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of USART1 clock */
    __HAL_RCC_USART1_CONFIG(PeriphClkInit->Usart1ClockSelection);
  }

  /*-------------------------- USART2 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_USART2) == RCC_PERIPHCLK_USART2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USART2CLKSOURCE(PeriphClkInit->Usart2ClockSelection));

    if (PeriphClkInit->Usart2ClockSelection == RCC_USART2CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Usart2ClockSelection == RCC_USART2CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Usart2ClockSelection == RCC_USART2CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of USART2 clock */
    __HAL_RCC_USART2_CONFIG(PeriphClkInit->Usart2ClockSelection);
  }

  /*-------------------------- USART3 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_USART3) == RCC_PERIPHCLK_USART3)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USART3CLKSOURCE(PeriphClkInit->Usart3ClockSelection));

    if (PeriphClkInit->Usart3ClockSelection == RCC_USART3CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Usart3ClockSelection == RCC_USART3CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Usart3ClockSelection == RCC_USART3CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of USART3 clock */
    __HAL_RCC_USART3_CONFIG(PeriphClkInit->Usart3ClockSelection);
  }

  /*-------------------------- UART4 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_UART4) == RCC_PERIPHCLK_UART4)
  {
    /* Check the parameters */
    assert_param(IS_RCC_UART4CLKSOURCE(PeriphClkInit->Uart4ClockSelection));

    if (PeriphClkInit->Uart4ClockSelection == RCC_UART4CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Uart4ClockSelection == RCC_UART4CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Uart4ClockSelection == RCC_UART4CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of UART4 clock */
    __HAL_RCC_UART4_CONFIG(PeriphClkInit->Uart4ClockSelection);
  }

  /*-------------------------- UART5 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_UART5) == RCC_PERIPHCLK_UART5)
  {
    /* Check the parameters */
    assert_param(IS_RCC_UART5CLKSOURCE(PeriphClkInit->Uart5ClockSelection));

    if (PeriphClkInit->Uart5ClockSelection == RCC_UART5CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Uart5ClockSelection == RCC_UART5CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Uart5ClockSelection == RCC_UART5CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of UART5 clock */
    __HAL_RCC_UART5_CONFIG(PeriphClkInit->Uart5ClockSelection);
  }

  /*-------------------------- USART6 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_USART6) == RCC_PERIPHCLK_USART6)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USART6CLKSOURCE(PeriphClkInit->Usart6ClockSelection));

    if (PeriphClkInit->Usart6ClockSelection == RCC_USART6CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Usart6ClockSelection == RCC_USART6CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Usart6ClockSelection == RCC_USART6CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of USART6 clock */
    __HAL_RCC_USART6_CONFIG(PeriphClkInit->Usart6ClockSelection);
  }

  /*-------------------------- UART7 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_UART7) == RCC_PERIPHCLK_UART7)
  {
    /* Check the parameters */
    assert_param(IS_RCC_UART7CLKSOURCE(PeriphClkInit->Uart7ClockSelection));

    if (PeriphClkInit->Uart7ClockSelection == RCC_UART7CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Uart7ClockSelection == RCC_UART7CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Uart7ClockSelection == RCC_UART7CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of UART7 clock */
    __HAL_RCC_UART7_CONFIG(PeriphClkInit->Uart7ClockSelection);
  }

  /*-------------------------- UART8 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_UART8) == RCC_PERIPHCLK_UART8)
  {
    /* Check the parameters */
    assert_param(IS_RCC_UART8CLKSOURCE(PeriphClkInit->Uart8ClockSelection));

    if (PeriphClkInit->Uart8ClockSelection == RCC_UART8CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Uart8ClockSelection == RCC_UART8CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Uart8ClockSelection == RCC_UART8CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of UART8 clock */
    __HAL_RCC_UART8_CONFIG(PeriphClkInit->Uart8ClockSelection);
  }

  /*-------------------------- UART9 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_UART9) == RCC_PERIPHCLK_UART9)
  {
    /* Check the parameters */
    assert_param(IS_RCC_UART9CLKSOURCE(PeriphClkInit->Uart9ClockSelection));

    if (PeriphClkInit->Uart9ClockSelection == RCC_UART9CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Uart9ClockSelection == RCC_UART9CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Uart9ClockSelection == RCC_UART9CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of UART9 clock */
    __HAL_RCC_UART9_CONFIG(PeriphClkInit->Uart9ClockSelection);
  }

  /*-------------------------- USART10 configuration --------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_USART10) == RCC_PERIPHCLK_USART10)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USART10CLKSOURCE(PeriphClkInit->Usart10ClockSelection));

    if (PeriphClkInit->Usart10ClockSelection == RCC_USART10CLKSOURCE_IC9)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC9].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC9].ClockDivider));

      /* Set IC9 configuration */
      MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL | RCC_IC9CFGR_IC9INT,
                 PeriphClkInit->ICSelection[RCC_IC9].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC9].ClockDivider - 1U) << RCC_IC9CFGR_IC9INT_Pos));

      LL_RCC_IC9_Enable();
    }
    else if (PeriphClkInit->Usart10ClockSelection == RCC_USART10CLKSOURCE_IC14)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC14].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC14].ClockDivider));

      /* Set IC14 configuration */
      MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL | RCC_IC14CFGR_IC14INT,
                 PeriphClkInit->ICSelection[RCC_IC14].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC14].ClockDivider - 1U) << RCC_IC14CFGR_IC14INT_Pos));

      LL_RCC_IC14_Enable();
    }
    else if (PeriphClkInit->Usart10ClockSelection == RCC_USART10CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of USART10 clock */
    __HAL_RCC_USART10_CONFIG(PeriphClkInit->Usart10ClockSelection);
  }

  /*------------------------------ USBPHY1 Configuration ---------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_USBPHY1) == RCC_PERIPHCLK_USBPHY1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USBPHY1CLKSOURCE(PeriphClkInit->UsbPhy1ClockSelection));

    /* Set the source of USBPHY1 clock*/
    __HAL_RCC_USBPHY1_CONFIG(PeriphClkInit->UsbPhy1ClockSelection);
  }

  /*------------------------------ USBPHY2 Configuration ---------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_USBPHY2) == RCC_PERIPHCLK_USBPHY2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USBPHY2CLKSOURCE(PeriphClkInit->UsbPhy2ClockSelection));

    /* Set the source of USBPHY2 clock*/
    __HAL_RCC_USBPHY2_CONFIG(PeriphClkInit->UsbPhy2ClockSelection);
  }

  /*------------------------------ USBOTGHS1 Configuration -------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_USBOTGHS1) == RCC_PERIPHCLK_USBOTGHS1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USBOTGHS1CLKSOURCE(PeriphClkInit->UsbOtgHs1ClockSelection));

    if (PeriphClkInit->UsbOtgHs1ClockSelection == RCC_USBOTGHS1CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->UsbOtgHs1ClockSelection == RCC_USBOTGHS1CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of USBOTGHS1 clock */
    __HAL_RCC_USBOTGHS1_CONFIG(PeriphClkInit->UsbOtgHs1ClockSelection);
  }

  /*------------------------------ USBOTGHS2 Configuration -------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_USBOTGHS2) == RCC_PERIPHCLK_USBOTGHS2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_USBOTGHS2CLKSOURCE(PeriphClkInit->UsbOtgHs2ClockSelection));

    if (PeriphClkInit->UsbOtgHs2ClockSelection == RCC_USBOTGHS2CLKSOURCE_IC15)
    {
      /* Check the parameters */
      assert_param(IS_RCC_ICCLKSOURCE(PeriphClkInit->ICSelection[RCC_IC15].ClockSelection));
      assert_param(IS_RCC_ICCLKDIVIDER(PeriphClkInit->ICSelection[RCC_IC15].ClockDivider));

      /* Set IC15 configuration */
      MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL | RCC_IC15CFGR_IC15INT,
                 PeriphClkInit->ICSelection[RCC_IC15].ClockSelection | \
                 ((PeriphClkInit->ICSelection[RCC_IC15].ClockDivider - 1U) << RCC_IC15CFGR_IC15INT_Pos));

      LL_RCC_IC15_Enable();
    }
    else if (PeriphClkInit->UsbOtgHs2ClockSelection == RCC_USBOTGHS2CLKSOURCE_CLKP)
    {
      LL_RCC_CLKP_Enable();
    }
    else
    {
      /* No specific enable to do on other sources */
    }

    /* Set the source of USBOTGHS2 clock */
    __HAL_RCC_USBOTGHS2_CONFIG(PeriphClkInit->UsbOtgHs2ClockSelection);
  }

  /*------------------------------------ TIM configuration --------------------------------------*/
  if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_TIM) == RCC_PERIPHCLK_TIM)
  {
    /* Check the parameters */
    assert_param(IS_RCC_TIMPRES(PeriphClkInit->TIMPresSelection));

    /* Configure Timer Prescaler */
    __HAL_RCC_TIMCLKPRESCALER_CONFIG(PeriphClkInit->TIMPresSelection);
  }

  if (status == HAL_OK)
  {
    return HAL_OK;
  }
  return HAL_ERROR;
}

/**
  * @brief  Get the RCC_ClkInitStruct according to the internal RCC configuration registers.
  * @param  PeriphClkInit: pointer to an RCC_PeriphCLKInitTypeDef structure that
  *         returns the configuration information for the Extended Peripherals clocks :
  *         (ADC, ADF1, CKPER, DCMI, DCMIPP, FDCAN, FMC, I2C1, I2C2, I2C3, I2C4, I3C1, I3C2,
  *         LPTIM1, LPTIM2, LPTIM3, LPTIM4, LPTIM5, LTDC, LPUART1, MDF1, PSSI, RTC, SAI1, SAI2,
  *         SDMMC1, SDMMC2, SPDIFRX1, SPI1, SPI2, SPI3, SPI4, SPI5, SPI6, TIM,
  *         USART1, USART2, USART3, UART4, UART5, USART6, UART7, UART8, UART9, USART10,
  *         USBPHYC, USB OTGFS, XSPI1, XSPI2, XSPI3).
  * @retval None
  */
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit)
{
  const __IO uint32_t *p_icxcfgr;
  uint32_t icx_val;
  /* Set all possible values for the extended clock type parameter------------*/
  PeriphClkInit->PeriphClockSelection = RCC_PERIPHCLK_ADC      | RCC_PERIPHCLK_ADF1      | RCC_PERIPHCLK_CKPER    | \
                                        RCC_PERIPHCLK_CSI      | RCC_PERIPHCLK_DCMIPP    | RCC_PERIPHCLK_ETH1     | \
                                        RCC_PERIPHCLK_ETH1PHY  | RCC_PERIPHCLK_ETH1RX    | RCC_PERIPHCLK_ETH1TX   | \
                                        RCC_PERIPHCLK_ETH1PTP  | RCC_PERIPHCLK_FDCAN     | RCC_PERIPHCLK_FMC      | \
                                        RCC_PERIPHCLK_I2C1     | RCC_PERIPHCLK_I2C2      | RCC_PERIPHCLK_I2C3     | \
                                        RCC_PERIPHCLK_I2C4     | RCC_PERIPHCLK_I3C1      | RCC_PERIPHCLK_I3C2     | \
                                        RCC_PERIPHCLK_LPTIM1   | RCC_PERIPHCLK_LPTIM2    | RCC_PERIPHCLK_LPTIM3   | \
                                        RCC_PERIPHCLK_LPTIM4   | RCC_PERIPHCLK_LPTIM5    | RCC_PERIPHCLK_LTDC     | \
                                        RCC_PERIPHCLK_LPUART1  | RCC_PERIPHCLK_MDF1      | RCC_PERIPHCLK_PSSI     | \
                                        RCC_PERIPHCLK_RTC      | RCC_PERIPHCLK_SAI1      | RCC_PERIPHCLK_SAI2     | \
                                        RCC_PERIPHCLK_SDMMC1   | RCC_PERIPHCLK_SDMMC2    | RCC_PERIPHCLK_SPDIFRX1 | \
                                        RCC_PERIPHCLK_SPI1     | RCC_PERIPHCLK_SPI2      | RCC_PERIPHCLK_SPI3     | \
                                        RCC_PERIPHCLK_SPI4     | RCC_PERIPHCLK_SPI5      | RCC_PERIPHCLK_SPI6     | \
                                        RCC_PERIPHCLK_TIM      | RCC_PERIPHCLK_USART1    | RCC_PERIPHCLK_USART2   | \
                                        RCC_PERIPHCLK_USART3   | RCC_PERIPHCLK_UART4     | RCC_PERIPHCLK_UART5    | \
                                        RCC_PERIPHCLK_USART6   | RCC_PERIPHCLK_UART7     | RCC_PERIPHCLK_UART8    | \
                                        RCC_PERIPHCLK_UART9    | RCC_PERIPHCLK_USART10   | \
                                        RCC_PERIPHCLK_XSPI1    | RCC_PERIPHCLK_XSPI2     | RCC_PERIPHCLK_XSPI3    | \
                                        RCC_PERIPHCLK_USBPHY1  | RCC_PERIPHCLK_USBOTGHS1 | \
                                        RCC_PERIPHCLK_USBPHY2  | RCC_PERIPHCLK_USBOTGHS2;

  /* Get IC x dividers */
  p_icxcfgr = &(RCC->IC1CFGR);
  for (uint32_t i = 0; i < 20U; i++)
  {
    icx_val = *p_icxcfgr;
    PeriphClkInit->ICSelection[i].ClockSelection = (icx_val & RCC_IC1CFGR_IC1SEL) >> RCC_IC1CFGR_IC1SEL_Pos;
    PeriphClkInit->ICSelection[i].ClockDivider = ((icx_val & RCC_IC1CFGR_IC1INT) >> RCC_IC1CFGR_IC1INT_Pos) + 1U;
    p_icxcfgr++;
  }

  /* Get the ADC clock source ------------------------------------------------*/
  PeriphClkInit->AdcClockSelection          = __HAL_RCC_GET_ADC_SOURCE();
  /* Get the ADC divider ------------------------------------------------*/
  PeriphClkInit->AdcDivider                 = __HAL_RCC_GET_ADC_DIVIDER();
  /* Get the ADF1 clock source -----------------------------------------------*/
  PeriphClkInit->Adf1ClockSelection         = __HAL_RCC_GET_ADF1_SOURCE();
  /* Get the CKPER clock source ----------------------------------------------*/
  PeriphClkInit->CkperClockSelection        = __HAL_RCC_GET_CLKP_SOURCE();
  /* Get the DCMIPP clock source ---------------------------------------------*/
  PeriphClkInit->DcmippClockSelection       = __HAL_RCC_GET_DCMIPP_SOURCE();
  /* Get the ETH1 clock source -----------------------------------------------*/
  PeriphClkInit->Eth1ClockSelection         = __HAL_RCC_GET_ETH1_SOURCE();
  /* Get the ETH1 PHY interface ----------------------------------------------*/
  PeriphClkInit->Eth1PhyInterfaceSelection  = __HAL_RCC_GET_ETH1PHY_INTERFACE();
  /* Get the ETH1 RX clock source --------------------------------------------*/
  PeriphClkInit->Eth1RxClockSelection       = __HAL_RCC_GET_ETH1RX_SOURCE();
  /* Get the ETH1 TX clock source --------------------------------------------*/
  PeriphClkInit->Eth1TxClockSelection       = __HAL_RCC_GET_ETH1TX_SOURCE();
  /* Get the ETH1 PTP clock source -------------------------------------------*/
  PeriphClkInit->Eth1PtpClockSelection      = __HAL_RCC_GET_ETH1PTP_SOURCE();
  /* Get the ETH1 PTP divider ------------------------------------------------*/
  PeriphClkInit->Eth1PtpDivider             = __HAL_RCC_GET_ETH1PTP_DIVIDER();
  /* Get the FDCAN kernel clock source ---------------------------------------*/
  PeriphClkInit->FdcanClockSelection        = __HAL_RCC_GET_FDCAN_SOURCE();
  /* Get the FMC kernel clock source -----------------------------------------*/
  PeriphClkInit->FmcClockSelection          = __HAL_RCC_GET_FMC_SOURCE();
  /* Get the I2C1 clock source -----------------------------------------------*/
  PeriphClkInit->I2c1ClockSelection         = __HAL_RCC_GET_I2C1_SOURCE();
  /* Get the I2C2 clock source -----------------------------------------------*/
  PeriphClkInit->I2c2ClockSelection         = __HAL_RCC_GET_I2C2_SOURCE();
  /* Get the I2C3 clock source -----------------------------------------------*/
  PeriphClkInit->I2c3ClockSelection         = __HAL_RCC_GET_I2C3_SOURCE();
  /* Get the I2C4 clock source -----------------------------------------------*/
  PeriphClkInit->I2c4ClockSelection         = __HAL_RCC_GET_I2C4_SOURCE();
  /* Get the I3C1 clock source -----------------------------------------------*/
  PeriphClkInit->I3c1ClockSelection         = __HAL_RCC_GET_I3C1_SOURCE();
  /* Get the I3C2 clock source -----------------------------------------------*/
  PeriphClkInit->I3c2ClockSelection         = __HAL_RCC_GET_I3C2_SOURCE();
  /* Get the LPTIM1 clock source ---------------------------------------------*/
  PeriphClkInit->Lptim1ClockSelection       = __HAL_RCC_GET_LPTIM1_SOURCE();
  /* Get the LPTIM2 clock source ---------------------------------------------*/
  PeriphClkInit->Lptim2ClockSelection       = __HAL_RCC_GET_LPTIM2_SOURCE();
  /* Get the LPTIM3 clock source ---------------------------------------------*/
  PeriphClkInit->Lptim3ClockSelection       = __HAL_RCC_GET_LPTIM3_SOURCE();
  /* Get the LPTIM4 clock source ---------------------------------------------*/
  PeriphClkInit->Lptim4ClockSelection       = __HAL_RCC_GET_LPTIM4_SOURCE();
  /* Get the LPTIM5 clock source ---------------------------------------------*/
  PeriphClkInit->Lptim5ClockSelection       = __HAL_RCC_GET_LPTIM5_SOURCE();
  /* Get the LPUART1 clock source --------------------------------------------*/
  PeriphClkInit->Lpuart1ClockSelection      = __HAL_RCC_GET_LPUART1_SOURCE();
  /* Get the LTDC clock source -----------------------------------------------*/
  PeriphClkInit->LtdcClockSelection         = __HAL_RCC_GET_LTDC_SOURCE();
  /* Get the MDF1 clock source -----------------------------------------------*/
  PeriphClkInit->Mdf1ClockSelection         = __HAL_RCC_GET_MDF1_SOURCE();
  /* Get the PSSI clock source -----------------------------------------------*/
  PeriphClkInit->PssiClockSelection         = __HAL_RCC_GET_PSSI_SOURCE();
  /* Get the RTC clock source ------------------------------------------------*/
  PeriphClkInit->RTCClockSelection          = __HAL_RCC_GET_RTC_SOURCE();
  /* Get the SAI1 clock source -----------------------------------------------*/
  PeriphClkInit->Sai1ClockSelection         = __HAL_RCC_GET_SAI1_SOURCE();
  /* Get the SAI2 clock source -----------------------------------------------*/
  PeriphClkInit->Sai2ClockSelection         = __HAL_RCC_GET_SAI2_SOURCE();
  /* Get the SDMMC1 clock source ---------------------------------------------*/
  PeriphClkInit->Sdmmc1ClockSelection       = __HAL_RCC_GET_SDMMC1_SOURCE();
  /* Get the SDMMC2 clock source ---------------------------------------------*/
  PeriphClkInit->Sdmmc2ClockSelection       = __HAL_RCC_GET_SDMMC2_SOURCE();
  /* Get the SPDIFRX1 clock source -------------------------------------------*/
  PeriphClkInit->Spdifrx1ClockSelection     = __HAL_RCC_GET_SPDIFRX1_SOURCE();
  /* Get the SPI1 clock source -----------------------------------------------*/
  PeriphClkInit->Spi1ClockSelection         = __HAL_RCC_GET_SPI1_SOURCE();
  /* Get the SPI2 clock source -----------------------------------------------*/
  PeriphClkInit->Spi2ClockSelection         = __HAL_RCC_GET_SPI2_SOURCE();
  /* Get the SPI3 clock source -----------------------------------------------*/
  PeriphClkInit->Spi3ClockSelection         = __HAL_RCC_GET_SPI3_SOURCE();
  /* Get the SPI4 clock source -----------------------------------------------*/
  PeriphClkInit->Spi4ClockSelection         = __HAL_RCC_GET_SPI4_SOURCE();
  /* Get the SPI5 clock source -----------------------------------------------*/
  PeriphClkInit->Spi5ClockSelection         = __HAL_RCC_GET_SPI5_SOURCE();
  /* Get the SPI6 clock source -----------------------------------------------*/
  PeriphClkInit->Spi6ClockSelection         = __HAL_RCC_GET_SPI6_SOURCE();
  /* Get the TIM Prescaler configuration -------------------------------------*/
  PeriphClkInit->TIMPresSelection           = __HAL_RCC_GET_TIMCLKPRESCALER();
  /* Get the USART1 clock source ---------------------------------------------*/
  PeriphClkInit->Usart1ClockSelection       = __HAL_RCC_GET_USART1_SOURCE();
  /* Get the USART2 clock source ---------------------------------------------*/
  PeriphClkInit->Usart2ClockSelection       = __HAL_RCC_GET_USART2_SOURCE();
  /* Get the USART3 clock source ---------------------------------------------*/
  PeriphClkInit->Usart3ClockSelection       = __HAL_RCC_GET_USART3_SOURCE();
  /* Get the UART4 clock source ----------------------------------------------*/
  PeriphClkInit->Uart4ClockSelection        = __HAL_RCC_GET_UART4_SOURCE();
  /* Get the UART5 clock source ----------------------------------------------*/
  PeriphClkInit->Uart5ClockSelection        = __HAL_RCC_GET_UART5_SOURCE();
  /* Get the USART6 clock source ---------------------------------------------*/
  PeriphClkInit->Usart6ClockSelection       = __HAL_RCC_GET_USART6_SOURCE();
  /* Get the UART7 clock source ----------------------------------------------*/
  PeriphClkInit->Uart7ClockSelection        = __HAL_RCC_GET_UART7_SOURCE();
  /* Get the UART8 clock source ----------------------------------------------*/
  PeriphClkInit->Uart8ClockSelection        = __HAL_RCC_GET_UART8_SOURCE();
  /* Get the UART9 clock source ----------------------------------------------*/
  PeriphClkInit->Uart9ClockSelection        = __HAL_RCC_GET_UART9_SOURCE();
  /* Get the USART10 clock source --------------------------------------------*/
  PeriphClkInit->Usart10ClockSelection      = __HAL_RCC_GET_USART10_SOURCE();
  /* Get the USBPHY1 clock source --------------------------------------------*/
  PeriphClkInit->UsbPhy1ClockSelection      = __HAL_RCC_GET_USBPHY1_SOURCE();
  /* Get the USB OTGHS1 clock source -----------------------------------------*/
  PeriphClkInit->UsbOtgHs1ClockSelection    = __HAL_RCC_GET_USBOTGHS1_SOURCE();
  /* Get the USBPHY2 clock source --------------------------------------------*/
  PeriphClkInit->UsbPhy2ClockSelection      = __HAL_RCC_GET_USBPHY2_SOURCE();
  /* Get the USB OTGHS2 clock source -----------------------------------------*/
  PeriphClkInit->UsbOtgHs2ClockSelection    = __HAL_RCC_GET_USBOTGHS2_SOURCE();
  /* Get the XSPI1 clock source ----------------------------------------------*/
  PeriphClkInit->Xspi1ClockSelection        = __HAL_RCC_GET_XSPI1_SOURCE();
  /* Get the XSPI2 clock source ----------------------------------------------*/
  PeriphClkInit->Xspi2ClockSelection        = __HAL_RCC_GET_XSPI2_SOURCE();
  /* Get the XSPI3 clock source ----------------------------------------------*/
  PeriphClkInit->Xspi3ClockSelection        = __HAL_RCC_GET_XSPI3_SOURCE();
}

/**
  * @brief  Return the peripheral clock frequency for a given peripheral (SAI..)
  * @note   Return 0 if peripheral clock identifier not managed by this API or
  *         if the selected clock source is not enabled (HSI, PLLs clock output..)
  * @param  PeriphClk: Peripheral clock identifier
  *         This parameter can be one of the following values:
  *            @arg RCC_PERIPHCLK_ADC      : ADC peripheral clock
  *            @arg RCC_PERIPHCLK_ADF1     : ADF1 peripheral clock
  *            @arg RCC_PERIPHCLK_CSI      : CSI peripheral clock
  *            @arg RCC_PERIPHCLK_DCMIPP   : DCMIPP peripheral clock
  *            @arg RCC_PERIPHCLK_I2C1     : I2C1 peripheral clock
  *            @arg RCC_PERIPHCLK_I2C2     : I2C2 peripheral clock
  *            @arg RCC_PERIPHCLK_I2C3     : I2C3 peripheral clock
  *            @arg RCC_PERIPHCLK_I2C4     : I2C4 peripheral clock
  *            @arg RCC_PERIPHCLK_I3C1     : I3C1 peripheral clock
  *            @arg RCC_PERIPHCLK_I3C2     : I3C2 peripheral clock
  *            @arg RCC_PERIPHCLK_FDCAN    : FDCAN peripheral clock
  *            @arg RCC_PERIPHCLK_FMC      : FMC peripheral clock
  *            @arg RCC_PERIPHCLK_LPTIM1   : LPTIM1 peripheral clock
  *            @arg RCC_PERIPHCLK_LPTIM2   : LPTIM2 peripheral clock
  *            @arg RCC_PERIPHCLK_LPTIM3   : LPTIM3 peripheral clock
  *            @arg RCC_PERIPHCLK_LPTIM4   : LPTIM4 peripheral clock
  *            @arg RCC_PERIPHCLK_LPTIM5   : LPTIM5 peripheral clock
  *            @arg RCC_PERIPHCLK_LPUART1  : LPUART1 peripheral clock
  *            @arg RCC_PERIPHCLK_LTDC     : LTDC peripheral clock
  *            @arg RCC_PERIPHCLK_MDF1     : MDF1 peripheral clock
  *            @arg RCC_PERIPHCLK_PSSI     : PSSI peripheral clock
  *            @arg RCC_PERIPHCLK_SAI1     : SAI1 peripheral clock
  *            @arg RCC_PERIPHCLK_SAI2     : SAI2 peripheral clock
  *            @arg RCC_PERIPHCLK_SDMMC1   : SDMMC1 peripheral clock
  *            @arg RCC_PERIPHCLK_SDMMC2   : SDMMC2 peripheral clock
  *            @arg RCC_PERIPHCLK_SPDIFRX1 : SPDIFRX1 peripheral clock
  *            @arg RCC_PERIPHCLK_SPI1     : SPI1 peripheral clock
  *            @arg RCC_PERIPHCLK_SPI2     : SPI2 peripheral clock
  *            @arg RCC_PERIPHCLK_SPI3     : SPI3 peripheral clock
  *            @arg RCC_PERIPHCLK_SPI4     : SPI4 peripheral clock
  *            @arg RCC_PERIPHCLK_SPI5     : SPI5 peripheral clock
  *            @arg RCC_PERIPHCLK_SPI6     : SPI6 peripheral clock
  *            @arg RCC_PERIPHCLK_USART1   : USART1 peripheral clock
  *            @arg RCC_PERIPHCLK_USART2   : USART2 peripheral clock
  *            @arg RCC_PERIPHCLK_USART3   : USART3 peripheral clock
  *            @arg RCC_PERIPHCLK_UART4    : UART4 peripheral clock
  *            @arg RCC_PERIPHCLK_UART5    : UART5 peripheral clock
  *            @arg RCC_PERIPHCLK_USART6   : USART6 peripheral clock
  *            @arg RCC_PERIPHCLK_UART7    : UART7 peripheral clock
  *            @arg RCC_PERIPHCLK_UART8    : UART8 peripheral clock
  *            @arg RCC_PERIPHCLK_UART9    : UART9 peripheral clock
  *            @arg RCC_PERIPHCLK_USART10  : USART10 peripheral clock
  *            @arg RCC_PERIPHCLK_XSPI1    : XSPI1 peripheral clock
  *            @arg RCC_PERIPHCLK_XSPI2    : XSPI2 peripheral clock
  *            @arg RCC_PERIPHCLK_XSPI3    : XSPI3 peripheral clock
  * @retval Frequency in KHz
  */
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint64_t PeriphClk)
{
  uint32_t frequency = 0;   /* Set to 0 for returned value if no source clock */

  switch (PeriphClk)
  {
    case RCC_PERIPHCLK_ADC:
      frequency = RCCEx_GetADCCLKFreq(LL_RCC_ADC_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_ADF1:
      frequency = RCCEx_GetADFCLKFreq(LL_RCC_ADF1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_CSI:
      frequency = RCCEx_GetCSICLKFreq();
      break;

    case RCC_PERIPHCLK_CKPER:
      frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_DCMIPP:
      frequency = RCCEx_GetDCMIPPCLKFreq(LL_RCC_DCMIPP_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_ETH1:
      frequency = RCCEx_GetETH1CLKFreq(LL_RCC_ETH1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_ETH1PTP:
      frequency = RCCEx_GetETH1PTPCLKFreq(LL_RCC_ETH1PTP_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_FDCAN:
      frequency = RCCEx_GetFDCANCLKFreq(LL_RCC_FDCAN_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_FMC:
      frequency = RCCEx_GetFMCCLKFreq(LL_RCC_FMC_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_I2C1:
      frequency = RCCEx_GetI2CCLKFreq(LL_RCC_I2C1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_I2C2:
      frequency = RCCEx_GetI2CCLKFreq(LL_RCC_I2C2_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_I2C3:
      frequency = RCCEx_GetI2CCLKFreq(LL_RCC_I2C3_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_I2C4:
      frequency = RCCEx_GetI2CCLKFreq(LL_RCC_I2C4_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_I3C1:
      frequency = RCCEx_GetI3CCLKFreq(LL_RCC_I3C1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_I3C2:
      frequency = RCCEx_GetI3CCLKFreq(LL_RCC_I3C2_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_LPTIM1:
      frequency = RCCEx_GetLPTIMCLKFreq(LL_RCC_LPTIM1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_LPTIM2:
      frequency = RCCEx_GetLPTIMCLKFreq(LL_RCC_LPTIM2_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_LPTIM3:
      frequency = RCCEx_GetLPTIMCLKFreq(LL_RCC_LPTIM3_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_LPTIM4:
      frequency = RCCEx_GetLPTIMCLKFreq(LL_RCC_LPTIM4_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_LPTIM5:
      frequency = RCCEx_GetLPTIMCLKFreq(LL_RCC_LPTIM5_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_LPUART1:
      frequency = RCCEx_GetLPUARTCLKFreq(LL_RCC_LPUART1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_LTDC:
      frequency = RCCEx_GetLTDCCLKFreq(LL_RCC_LTDC_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_MDF1:
      frequency = RCCEx_GetMDFCLKFreq(LL_RCC_MDF1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_PSSI:
      frequency = RCCEx_GetPSSICLKFreq(LL_RCC_PSSI_CLKSOURCE);
      break;

    case  RCC_PERIPHCLK_RTC:
      frequency = RCCEx_GetRTCCLKFreq();
      break;

    case RCC_PERIPHCLK_SAI1:
      frequency = RCCEx_GetSAICLKFreq(LL_RCC_SAI1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_SAI2:
      frequency = RCCEx_GetSAICLKFreq(LL_RCC_SAI2_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_SDMMC1:
      frequency = RCCEx_GetSDMMCCLKFreq(LL_RCC_SDMMC1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_SDMMC2:
      frequency = RCCEx_GetSDMMCCLKFreq(LL_RCC_SDMMC2_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_SPDIFRX1:
      frequency = RCCEx_GetSPDIFRXCLKFreq(LL_RCC_SPDIFRX1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_SPI1:
      frequency = RCCEx_GetSPICLKFreq(LL_RCC_SPI1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_SPI2:
      frequency = RCCEx_GetSPICLKFreq(LL_RCC_SPI2_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_SPI3:
      frequency = RCCEx_GetSPICLKFreq(LL_RCC_SPI3_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_SPI4:
      frequency = RCCEx_GetSPICLKFreq(LL_RCC_SPI4_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_SPI5:
      frequency = RCCEx_GetSPICLKFreq(LL_RCC_SPI5_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_SPI6:
      frequency = RCCEx_GetSPICLKFreq(LL_RCC_SPI6_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_USART1:
      frequency = RCCEx_GetUSARTCLKFreq(LL_RCC_USART1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_USART2:
      frequency = RCCEx_GetUSARTCLKFreq(LL_RCC_USART2_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_USART3:
      frequency = RCCEx_GetUSARTCLKFreq(LL_RCC_USART3_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_UART4:
      frequency = RCCEx_GetUARTCLKFreq(LL_RCC_UART4_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_UART5:
      frequency = RCCEx_GetUARTCLKFreq(LL_RCC_UART5_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_USART6:
      frequency = RCCEx_GetUSARTCLKFreq(LL_RCC_USART6_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_UART7:
      frequency = RCCEx_GetUARTCLKFreq(LL_RCC_UART7_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_UART8:
      frequency = RCCEx_GetUARTCLKFreq(LL_RCC_UART8_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_UART9:
      frequency = RCCEx_GetUARTCLKFreq(LL_RCC_UART9_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_USART10:
      frequency = RCCEx_GetUSARTCLKFreq(LL_RCC_USART10_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_USBPHY1:
      frequency = RCCEx_GetOTGPHYCKREFCLKFreq(LL_RCC_OTGPHY1CKREF_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_USBOTGHS1:
      frequency = RCCEx_GetOTGPHYCLKFreq(LL_RCC_OTGPHY1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_USBPHY2:
      frequency = RCCEx_GetOTGPHYCKREFCLKFreq(LL_RCC_OTGPHY2CKREF_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_USBOTGHS2:
      frequency = RCCEx_GetOTGPHYCLKFreq(LL_RCC_OTGPHY2_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_XSPI1:
      frequency = RCCEx_GetXSPICLKFreq(LL_RCC_XSPI1_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_XSPI2:
      frequency = RCCEx_GetXSPICLKFreq(LL_RCC_XSPI2_CLKSOURCE);
      break;

    case RCC_PERIPHCLK_XSPI3:
      frequency = RCCEx_GetXSPICLKFreq(LL_RCC_XSPI3_CLKSOURCE);
      break;

    default:
      /* Unexpected case, frequency is by default set to 0 */
      break;
  }

  return frequency;
}

/**
  * @brief  Return PLL1 clock frequency
  * @note   RCC_PERIPH_FREQUENCY_NO returned for non activated output or oscillator not ready
  * @retval PLL1 output clock frequency
  */
uint32_t HAL_RCCEx_GetPLL1CLKFreq(void)
{
  uint32_t plloutputfreq = RCC_PERIPH_FREQUENCY_NO;
  uint32_t divm;

  /* PLL_VCO = (HSE_VALUE, MSI_VALUE or HSI_VALUE/HSIDIV) / PLLM * (PLLN + FRACN)
     SYSCLK = PLL_VCO / PLLP1 / PLLP2
  */
  if (LL_RCC_PLL1_IsReady() != 0U)
  {
    if (LL_RCC_PLL1P_IsEnabled() != 0U)
    {
      uint32_t pllinputfreq = RCCEx_GetPLLSourceFreq(LL_RCC_PLL1_GetSource());

      if (pllinputfreq != RCC_PERIPH_FREQUENCY_NO)
      {
        divm = LL_RCC_PLL1_GetM();

        if (divm != 0U)
        {
          plloutputfreq = RCCEx_CalcPLLFreq(pllinputfreq, divm, LL_RCC_PLL1_GetN(), LL_RCC_PLL1_GetFRACN(), \
                                            LL_RCC_PLL1_GetP1(), LL_RCC_PLL1_GetP2());
        }
      }
    }
  }
  else if (LL_RCC_PLL1_IsEnabledBypass() != 0U)
  {
    plloutputfreq = RCCEx_GetPLLSourceFreq(LL_RCC_PLL1_GetSource());
  }
  else
  {
    /* Nothing to do */
  }

  return plloutputfreq;
}

/**
  * @brief  Return PLL2 clock frequency
  * @note   RCC_PERIPH_FREQUENCY_NO returned for non activated output or oscillator not ready
  * @retval PLL2 output clock frequency
  */
uint32_t HAL_RCCEx_GetPLL2CLKFreq(void)
{
  uint32_t plloutputfreq = RCC_PERIPH_FREQUENCY_NO;
  uint32_t divm;

  /* PLL_VCO = (HSE_VALUE, MSI_VALUE or HSI_VALUE/HSIDIV) / PLLM * (PLLN + FRACN)
     SYSCLK = PLL_VCO / PLLP1 / PLLP2
  */
  if (LL_RCC_PLL2_IsReady() != 0U)
  {
    if (LL_RCC_PLL2P_IsEnabled() != 0U)
    {
      uint32_t pllinputfreq = RCCEx_GetPLLSourceFreq(LL_RCC_PLL2_GetSource());

      if (pllinputfreq != RCC_PERIPH_FREQUENCY_NO)
      {

        divm = LL_RCC_PLL2_GetM();

        if (divm != 0U)
        {
          plloutputfreq = RCCEx_CalcPLLFreq(pllinputfreq, divm, LL_RCC_PLL2_GetN(), LL_RCC_PLL2_GetFRACN(), \
                                            LL_RCC_PLL2_GetP1(), LL_RCC_PLL2_GetP2());
        }
      }
    }
  }
  else if (LL_RCC_PLL2_IsEnabledBypass() != 0U)
  {
    plloutputfreq = RCCEx_GetPLLSourceFreq(LL_RCC_PLL2_GetSource());
  }
  else
  {
    /* Nothing to do */
  }

  return plloutputfreq;
}

/**
  * @brief  Return PLL3 clock frequency
  * @note   RCC_PERIPH_FREQUENCY_NO returned for non activated output or oscillator not ready
  * @retval PLL3 output clock frequency
  */
uint32_t HAL_RCCEx_GetPLL3CLKFreq(void)
{
  uint32_t plloutputfreq = RCC_PERIPH_FREQUENCY_NO;
  uint32_t divm;

  /* PLL_VCO = (HSE_VALUE, MSI_VALUE or HSI_VALUE/HSIDIV) / PLLM * (PLLN + FRACN)
     SYSCLK = PLL_VCO / PLLP1 / PLLP2
  */
  if (LL_RCC_PLL3_IsReady() != 0U)
  {
    if (LL_RCC_PLL3P_IsEnabled() != 0U)
    {
      uint32_t pllinputfreq = RCCEx_GetPLLSourceFreq(LL_RCC_PLL3_GetSource());

      if (pllinputfreq != RCC_PERIPH_FREQUENCY_NO)
      {
        divm = LL_RCC_PLL3_GetM();

        if (divm != 0U)
        {
          plloutputfreq = RCCEx_CalcPLLFreq(pllinputfreq, divm, LL_RCC_PLL3_GetN(), LL_RCC_PLL3_GetFRACN(), \
                                            LL_RCC_PLL3_GetP1(), LL_RCC_PLL3_GetP2());
        }
      }
    }
  }
  else if (LL_RCC_PLL3_IsEnabledBypass() != 0U)
  {
    plloutputfreq = RCCEx_GetPLLSourceFreq(LL_RCC_PLL3_GetSource());
  }
  else
  {
    /* Nothing to do */
  }

  return plloutputfreq;
}

/**
  * @brief  Return PLL4 clock frequency
  * @note   RCC_PERIPH_FREQUENCY_NO returned for non activated output or oscillator not ready
  * @retval PLL4 output clock frequency
  */
uint32_t HAL_RCCEx_GetPLL4CLKFreq(void)
{
  uint32_t plloutputfreq = RCC_PERIPH_FREQUENCY_NO;
  uint32_t divm;

  /* PLL_VCO = (HSE_VALUE, MSI_VALUE or HSI_VALUE/HSIDIV) / PLLM * (PLLN + FRACN)
     SYSCLK = PLL_VCO / PLLP1 / PLLP2
  */
  if (LL_RCC_PLL4_IsReady() != 0U)
  {
    if (LL_RCC_PLL4P_IsEnabled() != 0U)
    {
      uint32_t pllinputfreq = RCCEx_GetPLLSourceFreq(LL_RCC_PLL4_GetSource());

      if (pllinputfreq != RCC_PERIPH_FREQUENCY_NO)
      {

        divm = LL_RCC_PLL4_GetM();

        if (divm != 0U)
        {
          plloutputfreq = RCCEx_CalcPLLFreq(pllinputfreq, divm, LL_RCC_PLL4_GetN(), LL_RCC_PLL4_GetFRACN(), \
                                            LL_RCC_PLL4_GetP1(), LL_RCC_PLL4_GetP2());
        }
      }
    }
  }
  else if (LL_RCC_PLL4_IsEnabledBypass() != 0U)
  {
    plloutputfreq = RCCEx_GetPLLSourceFreq(LL_RCC_PLL4_GetSource());
  }
  else
  {
    /* Nothing to do */
  }

  return plloutputfreq;
}

/**
  * @}
  */

/** @defgroup RCCEx_Exported_Functions_Group2 Extended System Control functions
  *  @brief  Extended Peripheral Control functions
  * @{
  */
/**
  * @brief  Configure the oscillator clock source for wakeup from Stop and CSS backup clock
  * @param  WakeUpClk Wakeup clock
  *         This parameter can be one of the following values:
  *            @arg RCC_STOP_WAKEUPCLOCK_MSI: MSI oscillator selection
  *            @arg RCC_STOP_WAKEUPCLOCK_HSI: HSI oscillator selection
  * @note   This function shall not be called after the Clock Security System on HSE has been
  *         enabled and the system clock is HSE or a switch on HSE is requested.
  * @retval None
  */
void HAL_RCCEx_WakeUpStopCLKConfig(uint32_t WakeUpClk)
{
  assert_param(IS_RCC_STOP_WAKEUPCLOCK(WakeUpClk));

  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(WakeUpClk);
}

/**
  * @brief  Enable the LSE Clock Security System.
  * @note   Prior to enable the LSE Clock Security System, LSE oscillator is to be enabled
  *         with HAL_RCC_OscConfig() and the LSE oscillator clock is to be selected as RTC
  *         clock with HAL_RCCEx_PeriphCLKConfig().
  * @note   Backup domain access should be enabled
  * @retval None
  */
void HAL_RCCEx_EnableLSECSS(void)
{
  LL_RCC_LSE_EnableCSS();
}

/**
  * @brief  Disable the LSE Clock Security System.
  * @note   LSE Clock Security System can only be disabled after a LSE failure detection.
  * @note   Backup domain access should be enabled
  * @retval None
  */
void HAL_RCCEx_DisableLSECSS(void)
{
  /* Disable LSE CSS IT if any */
  __HAL_RCC_DISABLE_IT(RCC_IT_LSECSS);

  LL_RCC_LSE_DisableCSS();
}

/**
  * @brief  Enable the LSE Clock Security System Interrupt.
  * @note   LSE Clock Security System Interrupt is mapped on EXTI line 65 which must be
  *         enabled at application level to wake up from low power modes
  * @retval None
  */
void HAL_RCCEx_EnableLSECSS_IT(void)
{
  /* Enable LSE CSS IT */
  __HAL_RCC_ENABLE_IT(RCC_IT_LSECSS);

  LL_RCC_LSE_EnableCSS();
}

/**
  * @brief  Disable the LSE Clock Security System Interrupt.
  * @retval None
  */
void HAL_RCCEx_DisableLSECSS_IT(void)
{
  /* Disable LSE CSS IT */
  __HAL_RCC_DISABLE_IT(RCC_IT_LSECSS);

  LL_RCC_LSE_DisableCSS();
}

/**
  * @brief  Rearm the LSE Clock Security System
  * @note   Allow to re-arm the LSE Clock Security System after a LSE failure detection
  * @retval None
  */
void HAL_RCCEx_ReArmLSECSS(void)
{
  LL_RCC_LSE_ReArmCSS();
}

/**
  * @brief Handle the RCC LSE Clock Security System interrupt request.
  * @retval None
  */
void HAL_RCCEx_LSECSS_IRQHandler(void)
{
  /* Check RCC LSE CSSF flag  */
  if (__HAL_RCC_GET_IT(RCC_IT_LSECSS))
  {
    /* Clear RCC LSE CSS pending bit */
    __HAL_RCC_CLEAR_IT(RCC_IT_LSECSS);

    /* RCC LSE Clock Security System interrupt user callback */
    HAL_RCCEx_LSECSS_Callback();
  }
}

/**
  * @brief  RCCEx LSE Clock Security System interrupt callback.
  * @retval none
  */
__weak void HAL_RCCEx_LSECSS_Callback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the @ref HAL_RCCEx_LSECSS_Callback should be implemented in the user file
  */
}

/**
  * @brief  Configure the requested PLL to SSCG Mode
  * @param  PLLnumber PLL number to configure.
  *         This parameter can be one of the following values:
  *            @ref RCC_PLL_Selection
  * @param  pPLLInit Pointer to an RCC_PLLInitTypeDef structure that
  *                  contains the configuration parameters.
  * @param  pPLLSSCGInit Pointer to an RCC_PLLSSCGInitTypeDef structure that
  *                  contains the configuration parameters specific to SSCG.
  * @note   PLL is temporary disabled to apply new parameters
  *
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RCCEx_PLLSSCGConfig(uint32_t PLLnumber, const RCC_PLLInitTypeDef *pPLLInit,
                                          const RCC_PLLSSCGInitTypeDef *pPLLSSCGInit)
{
  __IO uint32_t *p_rcc_pll_cfgr1_reg;
  __IO uint32_t *p_rcc_pll_cfgr2_reg;
  __IO uint32_t *p_rcc_pll_cfgr3_reg;
  HAL_StatusTypeDef ret = HAL_OK;
  uint32_t tickstart;

  p_rcc_pll_cfgr1_reg = &(RCC->PLL1CFGR1) + (((uint32_t)0x4) * PLLnumber);
  p_rcc_pll_cfgr2_reg = &(RCC->PLL1CFGR2) + (((uint32_t)0x4) * PLLnumber);
  p_rcc_pll_cfgr3_reg = &(RCC->PLL1CFGR3) + (((uint32_t)0x4) * PLLnumber);

  if (pPLLInit->PLLState == RCC_PLL_ON)
  {
    /* Check the parameters */
    assert_param(IS_RCC_PLLSOURCE(pPLLInit->PLLSource));
    assert_param(IS_RCC_PLLFRACN_VALUE(pPLLInit->PLLFractional));
    assert_param(IS_RCC_PLLM_VALUE(pPLLInit->PLLM));
    assert_param(IS_RCC_PLLN_VALUE(pPLLInit->PLLN));
    assert_param(IS_RCC_PLLP_VALUE(pPLLInit->PLLP1));
    assert_param(IS_RCC_PLLP_VALUE(pPLLInit->PLLP2));
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

    /* Configure PLLx SSCG Parameters */
    MODIFY_REG(*p_rcc_pll_cfgr3_reg, (RCC_PLL1CFGR3_PLL1MODDIV | RCC_PLL1CFGR3_PLL1MODSPR | RCC_PLL1CFGR3_PLL1MODSPR), \
               ((pPLLSSCGInit->PLLModDiv << RCC_PLL1CFGR3_PLL1MODDIV_Pos) \
                | (pPLLSSCGInit->PLLModSpreadDepth << RCC_PLL1CFGR3_PLL1MODSPR_Pos)
                | (pPLLSSCGInit->PLLModSpreadMode << RCC_PLL1CFGR3_PLL1MODSPRDW_Pos)));

    /* Clear PLLxMODSSDIS, PLLxMODDSEN and DACEN to 0 */
    CLEAR_BIT(*p_rcc_pll_cfgr3_reg, RCC_PLL1CFGR3_PLL1MODSSDIS | RCC_PLL1CFGR3_PLL1MODDSEN | RCC_PLL1CFGR3_PLL1DACEN);

    /* Ensure PLLxMODSSRST='1' and Enable PLLx post divider output */
    SET_BIT(*p_rcc_pll_cfgr3_reg, (RCC_PLL1CFGR3_PLL1MODSSRST | RCC_PLL1CFGR3_PLL1PDIVEN));

    /* Set PLLxMODDSEN and DACEN */
    SET_BIT(*p_rcc_pll_cfgr3_reg, (RCC_PLL1CFGR3_PLL1MODDSEN | RCC_PLL1CFGR3_PLL1DACEN));

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

    /* When PLL is Ready, deassert the PLLxMODSSRST */
    CLEAR_BIT(*p_rcc_pll_cfgr3_reg, RCC_PLL1CFGR3_PLL1MODSSRST);
  }
  else if (pPLLInit->PLLState == RCC_PLL_BYPASS)
  {
    ret = HAL_ERROR;
  }
  else
  {
    /* other states are not managed this function */
  }

  return ret;
}
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup RCCEx_Private_Functions RCCEx Private Functions
  * @{
  */

/**
  * @brief  Return HCLK clock frequency
  * @param  SYSCLK_Frequency SYSCLK clock frequency
  * @retval HCLK clock frequency (in Hz)
  */
static uint32_t RCCEx_GetHCLKFreq(uint32_t SYSCLK_Frequency)
{
  /* HCLK clock frequency */
  return LL_RCC_CALC_HCLK_FREQ(SYSCLK_Frequency, LL_RCC_GetAHBPrescaler());
}

/**
  * @brief  Return PCLK1 clock frequency
  * @param  HCLK_Frequency HCLK clock frequency
  * @retval PCLK1 clock frequency (in Hz)
  */
static uint32_t RCCEx_GetPCLK1Freq(uint32_t HCLK_Frequency)
{
  /* PCLK1 clock frequency */
  return LL_RCC_CALC_PCLK1_FREQ(HCLK_Frequency, LL_RCC_GetAPB1Prescaler());
}

/**
  * @brief  Return PCLK2 clock frequency
  * @param  HCLK_Frequency HCLK clock frequency
  * @retval PCLK2 clock frequency (in Hz)
  */
static uint32_t RCCEx_GetPCLK2Freq(uint32_t HCLK_Frequency)
{
  /* PCLK2 clock frequency */
  return LL_RCC_CALC_PCLK2_FREQ(HCLK_Frequency, LL_RCC_GetAPB2Prescaler());
}

/**
  * @brief  Return PCLK4 clock frequency
  * @param  HCLK_Frequency HCLK clock frequency
  * @retval PCLK4 clock frequency (in Hz)
  */
static uint32_t RCCEx_GetPCLK4Freq(uint32_t HCLK_Frequency)
{
  /* PCLK4 clock frequency */
  return LL_RCC_CALC_PCLK4_FREQ(HCLK_Frequency, LL_RCC_GetAPB4Prescaler());
}

/**
  * @brief  Return PCLK5 clock frequency
  * @param  HCLK_Frequency HCLK clock frequency
  * @retval PCLK5 clock frequency (in Hz)
  */
static uint32_t RCCEx_GetPCLK5Freq(uint32_t HCLK_Frequency)
{
  /* PCLK5 clock frequency */
  return LL_RCC_CALC_PCLK5_FREQ(HCLK_Frequency, LL_RCC_GetAPB5Prescaler());
}

/**
  * @brief  Return PLL source clock frequency
  * @param  PLLsource PLL source clock
  * @retval PLL source clock frequency (in Hz)
  */
static uint32_t RCCEx_GetPLLSourceFreq(uint32_t PLLsource)
{
  uint32_t pllinputfreq = RCC_PERIPH_FREQUENCY_NO;

  switch (PLLsource)
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
        if (LL_RCC_MSI_GetFrequency() == LL_RCC_MSI_FREQ_4MHZ)
        {
          pllinputfreq = MSI_VALUE;
        }
        else
        {
          pllinputfreq = 16000000UL;
        }
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

  return pllinputfreq;
}

/**
  * @brief  Calculate the PLL frequency output when used in integer or fractional mode
  * @param  PLLInputFreq PLL Input frequency (based on HSE/(HSI/HSIDIV)/MSI)
  * @param  M      Between 1 and 63
  * @param  N      Between 16 and 640 in integer mode, 20 to 320 in fractional mode
  * @param  FRACN  0 in integer mode, between 0 and 0xFFFFFF in fractional mode
  * @param  P1     VCO output divider P1 between 1 and 7
  * @param  P2     VCO output divider P2 between 1 and 7
  * @retval PLL clock frequency (in Hz)
  */
static uint32_t RCCEx_CalcPLLFreq(uint32_t PLLInputFreq, uint32_t M, uint32_t N, uint32_t FRACN, uint32_t P1,
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
  *         @arg @ref RCCEx_ADC_Clock_Source
  * @retval ADC clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetADCCLKFreq(uint32_t ADCxSource)
{
  uint32_t adc_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetADCClockSource(ADCxSource))
  {
    case LL_RCC_ADC_CLKSOURCE_HCLK:
      adc_frequency = RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq());
      break;

    case LL_RCC_ADC_CLKSOURCE_CLKP:
      adc_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_ADC_CLKSOURCE_IC7:
      if (LL_RCC_IC7_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC7_GetDivider();
        switch (LL_RCC_IC7_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            adc_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            adc_frequency = adc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            adc_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            adc_frequency = adc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            adc_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            adc_frequency = adc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            adc_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            adc_frequency = adc_frequency / ic_divider;
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
            adc_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            adc_frequency = adc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            adc_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            adc_frequency = adc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            adc_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            adc_frequency = adc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            adc_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            adc_frequency = adc_frequency / ic_divider;
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
        adc_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_ADC_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        adc_frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_ADC_CLKSOURCE_I2S_CKIN:
      adc_frequency = EXTERNAL_CLOCK_VALUE;
      break;

    case LL_RCC_ADC_CLKSOURCE_TIMG:
      adc_frequency = HAL_RCC_GetSysClockFreq() / (1UL << LL_RCC_GetTIMPrescaler());
      break;

    default:
      /* Unexpected case */
      break;
  }

  return (adc_frequency / __HAL_RCC_GET_ADC_DIVIDER());
}

/**
  * @brief  Return ADFx clock frequency
  * @param  ADFxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_ADF1_Clock_Source
  * @retval ADF clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetADFCLKFreq(uint32_t ADFxSource)
{
  uint32_t adf_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetADFClockSource(ADFxSource))
  {
    case LL_RCC_ADF1_CLKSOURCE_HCLK:
      adf_frequency = RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq());
      break;

    case LL_RCC_ADF1_CLKSOURCE_CLKP:
      adf_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_ADF1_CLKSOURCE_IC7:
      if (LL_RCC_IC7_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC7_GetDivider();
        switch (LL_RCC_IC7_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            adf_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            adf_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            adf_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            adf_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
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
            adf_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            adf_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            adf_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            adf_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
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
        adf_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_ADF1_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        adf_frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_ADF1_CLKSOURCE_I2S_CKIN:
      adf_frequency = EXTERNAL_CLOCK_VALUE;
      break;

    case LL_RCC_ADF1_CLKSOURCE_TIMG:
      adf_frequency = HAL_RCC_GetSysClockFreq() / (1UL << LL_RCC_GetTIMPrescaler());
      break;

    default:
      /* Unexpected case */
      break;
  }

  return adf_frequency;
}

/**
  * @brief  Return CLKP clock frequency
  * @param  CLKPxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_CLKP_Clock_Source
  * @retval CLKP clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetCLKPCLKFreq(uint32_t CLKPxSource)
{
  uint32_t clkp_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  if (LL_RCC_CLKP_IsEnabled() == 1U)
  {
    switch (LL_RCC_GetCLKPClockSource(CLKPxSource))
    {
      case LL_RCC_CLKP_CLKSOURCE_HSI:
        if (LL_RCC_HSI_IsReady() != 0U)
        {
          clkp_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
        }
        break;

      case LL_RCC_CLKP_CLKSOURCE_MSI:
        if (LL_RCC_MSI_IsReady() != 0U)
        {
          clkp_frequency = MSI_VALUE;
        }
        break;

      case LL_RCC_CLKP_CLKSOURCE_HSE:
        if (LL_RCC_HSE_IsReady() != 0U)
        {
          clkp_frequency = HSE_VALUE;
        }
        break;

      case LL_RCC_CLKP_CLKSOURCE_IC5:
        if (LL_RCC_IC5_IsEnabled() != 0U)
        {
          ic_divider = LL_RCC_IC5_GetDivider();
          switch (LL_RCC_IC5_GetSource())
          {
            case LL_RCC_ICCLKSOURCE_PLL1:
              clkp_frequency = HAL_RCCEx_GetPLL1CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL2:
              clkp_frequency = HAL_RCCEx_GetPLL2CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL3:
              clkp_frequency = HAL_RCCEx_GetPLL3CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL4:
              clkp_frequency = HAL_RCCEx_GetPLL4CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
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
              clkp_frequency = HAL_RCCEx_GetPLL1CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL2:
              clkp_frequency = HAL_RCCEx_GetPLL2CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL3:
              clkp_frequency = HAL_RCCEx_GetPLL3CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL4:
              clkp_frequency = HAL_RCCEx_GetPLL4CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
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
              clkp_frequency = HAL_RCCEx_GetPLL1CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL2:
              clkp_frequency = HAL_RCCEx_GetPLL2CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL3:
              clkp_frequency = HAL_RCCEx_GetPLL3CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL4:
              clkp_frequency = HAL_RCCEx_GetPLL4CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
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
              clkp_frequency = HAL_RCCEx_GetPLL1CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL2:
              clkp_frequency = HAL_RCCEx_GetPLL2CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL3:
              clkp_frequency = HAL_RCCEx_GetPLL3CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL4:
              clkp_frequency = HAL_RCCEx_GetPLL4CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
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
              clkp_frequency = HAL_RCCEx_GetPLL1CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL2:
              clkp_frequency = HAL_RCCEx_GetPLL2CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL3:
              clkp_frequency = HAL_RCCEx_GetPLL3CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
              break;
            case LL_RCC_ICCLKSOURCE_PLL4:
              clkp_frequency = HAL_RCCEx_GetPLL4CLKFreq();
              clkp_frequency = clkp_frequency / ic_divider;
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
  }

  return clkp_frequency;
}

/**
  * @brief  Return CSI clock frequency
  * @retval CLKP clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetCSICLKFreq(void)
{
  uint32_t clkp_frequency = RCC_PERIPH_FREQUENCY_NO;

  if (LL_RCC_IC18_IsEnabled() != 0U)
  {
    uint32_t ic_divider = LL_RCC_IC18_GetDivider();
    switch (LL_RCC_IC18_GetSource())
    {
      case LL_RCC_ICCLKSOURCE_PLL1:
        clkp_frequency = HAL_RCCEx_GetPLL1CLKFreq();
        clkp_frequency = clkp_frequency / ic_divider;
        break;
      case LL_RCC_ICCLKSOURCE_PLL2:
        clkp_frequency = HAL_RCCEx_GetPLL2CLKFreq();
        clkp_frequency = clkp_frequency / ic_divider;
        break;
      case LL_RCC_ICCLKSOURCE_PLL3:
        clkp_frequency = HAL_RCCEx_GetPLL3CLKFreq();
        clkp_frequency = clkp_frequency / ic_divider;
        break;
      case LL_RCC_ICCLKSOURCE_PLL4:
        clkp_frequency = HAL_RCCEx_GetPLL4CLKFreq();
        clkp_frequency = clkp_frequency / ic_divider;
        break;
      default:
        /* Unexpected case */
        break;
    }
  }

  return clkp_frequency;
}

/**
  * @brief  Return DCMIPP clock frequency
  * @param  DCMIPPxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_DCMIPP_Clock_Source
  * @retval DCMIPP clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetDCMIPPCLKFreq(uint32_t DCMIPPxSource)
{
  uint32_t dcmipp_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetDCMIPPClockSource(DCMIPPxSource))
  {
    case LL_RCC_DCMIPP_CLKSOURCE_PCLK5:
      dcmipp_frequency = RCCEx_GetPCLK5Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_DCMIPP_CLKSOURCE_CLKP:
      dcmipp_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_DCMIPP_CLKSOURCE_IC17:
      if (LL_RCC_IC17_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC17_GetDivider();
        switch (LL_RCC_IC17_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            dcmipp_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            dcmipp_frequency = dcmipp_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            dcmipp_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            dcmipp_frequency = dcmipp_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            dcmipp_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            dcmipp_frequency = dcmipp_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            dcmipp_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            dcmipp_frequency = dcmipp_frequency / ic_divider;
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
        dcmipp_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return dcmipp_frequency;
}

/**
  * @brief  Return ETH1 clock frequency
  * @param  ETH1xSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_ETH1_Clock_Source
  * @retval ETH1 clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetETH1CLKFreq(uint32_t ETH1xSource)
{
  uint32_t eth1_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetETHClockSource(ETH1xSource))
  {
    case LL_RCC_ETH1_CLKSOURCE_HCLK:
      eth1_frequency = RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq());
      break;

    case LL_RCC_ETH1_CLKSOURCE_CLKP:
      eth1_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_ETH1_CLKSOURCE_IC12:
      if (LL_RCC_IC12_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC12_GetDivider();
        switch (LL_RCC_IC12_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            eth1_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            eth1_frequency = eth1_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            eth1_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            eth1_frequency = eth1_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            eth1_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            eth1_frequency = eth1_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            eth1_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            eth1_frequency = eth1_frequency / ic_divider;
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
        eth1_frequency = HSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return eth1_frequency;
}

/**
  * @brief  Return ETH1PTP clock frequency
  * @param  ETH1PTPxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_ETH1_PTP_Clock_Source
  * @retval ETH1PTP clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetETH1PTPCLKFreq(uint32_t ETH1PTPxSource)
{
  uint32_t eth1ptp_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetETHPTPClockSource(ETH1PTPxSource))
  {
    case LL_RCC_ETH1PTP_CLKSOURCE_HCLK:
      eth1ptp_frequency = RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq());
      break;

    case LL_RCC_ETH1PTP_CLKSOURCE_CLKP:
      eth1ptp_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_ETH1PTP_CLKSOURCE_IC13:
      if (LL_RCC_IC13_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC13_GetDivider();
        switch (LL_RCC_IC13_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            eth1ptp_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            eth1ptp_frequency = eth1ptp_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            eth1ptp_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            eth1ptp_frequency = eth1ptp_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            eth1ptp_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            eth1ptp_frequency = eth1ptp_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            eth1ptp_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            eth1ptp_frequency = eth1ptp_frequency / ic_divider;
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
        eth1ptp_frequency = HSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return (eth1ptp_frequency / __HAL_RCC_GET_ETH1PTP_DIVIDER());
}

/**
  * @brief  Return FDCAN clock frequency
  * @param  FDCANxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_FDCAN_Clock_Source
  * @retval FDCAN clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetFDCANCLKFreq(uint32_t FDCANxSource)
{
  uint32_t fdcan_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetFDCANClockSource(FDCANxSource))
  {
    case LL_RCC_FDCAN_CLKSOURCE_PCLK1:
      fdcan_frequency = RCCEx_GetPCLK1Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_FDCAN_CLKSOURCE_CLKP:
      fdcan_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_FDCAN_CLKSOURCE_IC19:
      if (LL_RCC_IC19_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC19_GetDivider();
        switch (LL_RCC_IC19_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            fdcan_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            fdcan_frequency = fdcan_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            fdcan_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            fdcan_frequency = fdcan_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            fdcan_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            fdcan_frequency = fdcan_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            fdcan_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            fdcan_frequency = fdcan_frequency / ic_divider;
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
        fdcan_frequency = HSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return fdcan_frequency;
}

/**
  * @brief  Return FMC clock frequency
  * @param  FMCxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_FMC_Clock_Source
  * @retval FMC clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetFMCCLKFreq(uint32_t FMCxSource)
{
  uint32_t fmc_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetFMCClockSource(FMCxSource))
  {
    case LL_RCC_FMC_CLKSOURCE_HCLK:
      fmc_frequency = RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq());
      break;

    case LL_RCC_FMC_CLKSOURCE_CLKP:
      fmc_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_FMC_CLKSOURCE_IC3:
      if (LL_RCC_IC3_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC3_GetDivider();
        switch (LL_RCC_IC3_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            fmc_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            fmc_frequency = fmc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            fmc_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            fmc_frequency = fmc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            fmc_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            fmc_frequency = fmc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            fmc_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            fmc_frequency = fmc_frequency / ic_divider;
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
            fmc_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            fmc_frequency = fmc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            fmc_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            fmc_frequency = fmc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            fmc_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            fmc_frequency = fmc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            fmc_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            fmc_frequency = fmc_frequency / ic_divider;
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

  return fmc_frequency;
}

/**
  * @brief  Return I2Cx clock frequency
  * @param  I2CxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_I2C1_Clock_Source
  *         @arg @ref RCCEx_I2C2_Clock_Source
  *         @arg @ref RCCEx_I2C3_Clock_Source
  *         @arg @ref RCCEx_I2C4_Clock_Source
  * @retval I2C clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetI2CCLKFreq(uint32_t I2CxSource)
{
  uint32_t i2c_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetI2CClockSource(I2CxSource))
  {
    case LL_RCC_I2C1_CLKSOURCE_PCLK1:
    case LL_RCC_I2C2_CLKSOURCE_PCLK1:
    case LL_RCC_I2C3_CLKSOURCE_PCLK1:
    case LL_RCC_I2C4_CLKSOURCE_PCLK1:
      i2c_frequency = RCCEx_GetPCLK1Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_I2C1_CLKSOURCE_CLKP:
    case LL_RCC_I2C2_CLKSOURCE_CLKP:
    case LL_RCC_I2C3_CLKSOURCE_CLKP:
    case LL_RCC_I2C4_CLKSOURCE_CLKP:
      i2c_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
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
            i2c_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            i2c_frequency = i2c_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            i2c_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            i2c_frequency = i2c_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            i2c_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            i2c_frequency = i2c_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            i2c_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            i2c_frequency = i2c_frequency / ic_divider;
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
            i2c_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            i2c_frequency = i2c_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            i2c_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            i2c_frequency = i2c_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            i2c_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            i2c_frequency = i2c_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            i2c_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            i2c_frequency = i2c_frequency / ic_divider;
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
        i2c_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_I2C1_CLKSOURCE_MSI:
    case LL_RCC_I2C2_CLKSOURCE_MSI:
    case LL_RCC_I2C3_CLKSOURCE_MSI:
    case LL_RCC_I2C4_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        i2c_frequency = MSI_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return i2c_frequency;
}

/**
  * @brief  Return I3Cx clock frequency
  * @param  I3CxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_I3C1_Clock_Source
  *         @arg @ref RCCEx_I3C2_Clock_Source
  * @retval I3C clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetI3CCLKFreq(uint32_t I3CxSource)
{
  uint32_t i3c_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetI3CClockSource(I3CxSource))
  {
    case LL_RCC_I3C1_CLKSOURCE_PCLK1:
    case LL_RCC_I3C2_CLKSOURCE_PCLK1:
      i3c_frequency = RCCEx_GetPCLK1Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_I3C1_CLKSOURCE_CLKP:
    case LL_RCC_I3C2_CLKSOURCE_CLKP:
      i3c_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_I3C1_CLKSOURCE_IC10:
    case LL_RCC_I3C2_CLKSOURCE_IC10:
      if (LL_RCC_IC10_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC10_GetDivider();
        switch (LL_RCC_IC10_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            i3c_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            i3c_frequency = i3c_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            i3c_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            i3c_frequency = i3c_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            i3c_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            i3c_frequency = i3c_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            i3c_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            i3c_frequency = i3c_frequency / ic_divider;
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
            i3c_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            i3c_frequency = i3c_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            i3c_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            i3c_frequency = i3c_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            i3c_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            i3c_frequency = i3c_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            i3c_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            i3c_frequency = i3c_frequency / ic_divider;
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
        i3c_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_I3C1_CLKSOURCE_MSI:
    case LL_RCC_I3C2_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        i3c_frequency = MSI_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return i3c_frequency;
}

/**
  * @brief  Return LPTIMx clock frequency
  * @param  LPTIMxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_LPTIM1_Clock_Source
  *         @arg @ref RCCEx_LPTIM2_Clock_Source
  *         @arg @ref RCCEx_LPTIM3_Clock_Source
  *         @arg @ref RCCEx_LPTIM4_Clock_Source
  *         @arg @ref RCCEx_LPTIM5_Clock_Source
  * @retval LPTIM clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetLPTIMCLKFreq(uint32_t LPTIMxSource)
{
  uint32_t lptim_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetLPTIMClockSource(LPTIMxSource))
  {
    case LL_RCC_LPTIM1_CLKSOURCE_PCLK1:
      lptim_frequency = RCCEx_GetPCLK1Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_LPTIM2_CLKSOURCE_PCLK4:
    case LL_RCC_LPTIM3_CLKSOURCE_PCLK4:
    case LL_RCC_LPTIM4_CLKSOURCE_PCLK4:
    case LL_RCC_LPTIM5_CLKSOURCE_PCLK4:
      lptim_frequency = RCCEx_GetPCLK4Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_LPTIM1_CLKSOURCE_CLKP:
    case LL_RCC_LPTIM2_CLKSOURCE_CLKP:
    case LL_RCC_LPTIM3_CLKSOURCE_CLKP:
    case LL_RCC_LPTIM4_CLKSOURCE_CLKP:
    case LL_RCC_LPTIM5_CLKSOURCE_CLKP:
      lptim_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
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
            lptim_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            lptim_frequency = lptim_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            lptim_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            lptim_frequency = lptim_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            lptim_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            lptim_frequency = lptim_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            lptim_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            lptim_frequency = lptim_frequency / ic_divider;
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
        lptim_frequency = LSE_VALUE;
      }
      break;

    case LL_RCC_LPTIM1_CLKSOURCE_LSI:
    case LL_RCC_LPTIM2_CLKSOURCE_LSI:
    case LL_RCC_LPTIM3_CLKSOURCE_LSI:
    case LL_RCC_LPTIM4_CLKSOURCE_LSI:
    case LL_RCC_LPTIM5_CLKSOURCE_LSI:
      if (LL_RCC_LSI_IsReady() != 0U)
      {
        lptim_frequency = LSI_VALUE;
      }
      break;

    case LL_RCC_LPTIM1_CLKSOURCE_TIMG:
    case LL_RCC_LPTIM2_CLKSOURCE_TIMG:
    case LL_RCC_LPTIM3_CLKSOURCE_TIMG:
    case LL_RCC_LPTIM4_CLKSOURCE_TIMG:
    case LL_RCC_LPTIM5_CLKSOURCE_TIMG:
      lptim_frequency = HAL_RCC_GetSysClockFreq() / (1UL << LL_RCC_GetTIMPrescaler());
      break;

    default:
      /* Unexpected case */
      break;
  }

  return lptim_frequency;
}

/**
  * @brief  Return LPUART clock frequency
  * @param  LPUARTxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_LPUART1_Clock_Source
  * @retval LPUART clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetLPUARTCLKFreq(uint32_t LPUARTxSource)
{
  uint32_t lpuart_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetLPUARTClockSource(LPUARTxSource))
  {
    case LL_RCC_LPUART1_CLKSOURCE_PCLK4:
      lpuart_frequency = RCCEx_GetPCLK4Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_LPUART1_CLKSOURCE_CLKP:
      lpuart_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_LPUART1_CLKSOURCE_IC9:
      if (LL_RCC_IC9_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC9_GetDivider();
        switch (LL_RCC_IC9_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            lpuart_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            lpuart_frequency = lpuart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            lpuart_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            lpuart_frequency = lpuart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            lpuart_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            lpuart_frequency = lpuart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            lpuart_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            lpuart_frequency = lpuart_frequency / ic_divider;
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
            lpuart_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            lpuart_frequency = lpuart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            lpuart_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            lpuart_frequency = lpuart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            lpuart_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            lpuart_frequency = lpuart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            lpuart_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            lpuart_frequency = lpuart_frequency / ic_divider;
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
        lpuart_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_LPUART1_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        lpuart_frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_LPUART1_CLKSOURCE_LSE:
      if (LL_RCC_LSE_IsReady() != 0U)
      {
        lpuart_frequency = LSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return lpuart_frequency;
}

/**
  * @brief  Return LTDC clock frequency
  * @param  LTDCxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_LTDC_Clock_Source
  * @retval LTDC clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetLTDCCLKFreq(uint32_t LTDCxSource)
{
  uint32_t ltdc_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetLTDCClockSource(LTDCxSource))
  {
    case LL_RCC_LTDC_CLKSOURCE_PCLK5:
      ltdc_frequency = RCCEx_GetPCLK5Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_LTDC_CLKSOURCE_CLKP:
      ltdc_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_LTDC_CLKSOURCE_IC16:
      if (LL_RCC_IC16_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC16_GetDivider();
        switch (LL_RCC_IC16_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            ltdc_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            ltdc_frequency = ltdc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            ltdc_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            ltdc_frequency = ltdc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            ltdc_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            ltdc_frequency = ltdc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            ltdc_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            ltdc_frequency = ltdc_frequency / ic_divider;
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
        ltdc_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return ltdc_frequency;
}

/**
  * @brief  Return MDFx clock frequency
  * @param  MDFxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_MDF1_Clock_Source
  * @retval MDF clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetMDFCLKFreq(uint32_t MDFxSource)
{
  uint32_t adf_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetMDFClockSource(MDFxSource))
  {
    case LL_RCC_MDF1_CLKSOURCE_HCLK:
      adf_frequency = RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq());
      break;

    case LL_RCC_MDF1_CLKSOURCE_CLKP:
      adf_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_MDF1_CLKSOURCE_IC7:
      if (LL_RCC_IC7_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC7_GetDivider();
        switch (LL_RCC_IC7_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            adf_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            adf_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            adf_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            adf_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
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
            adf_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            adf_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            adf_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            adf_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            adf_frequency = adf_frequency / ic_divider;
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
        adf_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_MDF1_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        adf_frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_MDF1_CLKSOURCE_I2S_CKIN:
      adf_frequency = EXTERNAL_CLOCK_VALUE;
      break;

    case LL_RCC_MDF1_CLKSOURCE_TIMG:
      adf_frequency = HAL_RCC_GetSysClockFreq() / (1UL << LL_RCC_GetTIMPrescaler());
      break;

    default:
      /* Unexpected case */
      break;
  }

  return adf_frequency;
}

/**
  * @brief  Return PSSI clock frequency
  * @param  PSSIxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_PSSI_Clock_Source
  * @retval PSSI clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetPSSICLKFreq(uint32_t PSSIxSource)
{
  uint32_t pssi_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetPSSIClockSource(PSSIxSource))
  {
    case LL_RCC_PSSI_CLKSOURCE_HCLK:
      pssi_frequency = RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq());
      break;

    case LL_RCC_PSSI_CLKSOURCE_CLKP:
      pssi_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_PSSI_CLKSOURCE_IC20:
      if (LL_RCC_IC20_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC20_GetDivider();
        switch (LL_RCC_IC20_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            pssi_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            pssi_frequency = pssi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            pssi_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            pssi_frequency = pssi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            pssi_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            pssi_frequency = pssi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            pssi_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            pssi_frequency = pssi_frequency / ic_divider;
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
        pssi_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return pssi_frequency;
}

/**
  * @brief  Return RTC clock frequency
  * @retval RTC clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetRTCCLKFreq()
{
  uint32_t rtc_frequency = RCC_PERIPH_FREQUENCY_NO;

  switch (LL_RCC_GetRTCClockSource())
  {
    case LL_RCC_RTC_CLKSOURCE_NONE:
      /* Nothing to do */
      break;

    case LL_RCC_RTC_CLKSOURCE_LSE:
      if (LL_RCC_LSE_IsReady() != 0U)
      {
        rtc_frequency = LSE_VALUE;
      }
      break;

    case LL_RCC_RTC_CLKSOURCE_LSI:
      if (LL_RCC_LSI_IsReady() != 0U)
      {
        rtc_frequency = LSI_VALUE;
      }
      break;

    case LL_RCC_RTC_CLKSOURCE_HSE:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        uint32_t prescaler = (READ_BIT(RCC->CCIPR7, RCC_CCIPR7_RTCPRE) >> RCC_CCIPR7_RTCPRE_Pos) + 1U;
        rtc_frequency = HSE_VALUE / prescaler;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return rtc_frequency;
}

/**
  * @brief  Return SAIx clock frequency
  * @param  SAIxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_SAI1_Clock_Source
  *         @arg @ref RCCEx_SAI2_Clock_Source
  * @retval SAI clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetSAICLKFreq(uint32_t SAIxSource)
{
  uint32_t sai_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetSAIClockSource(SAIxSource))
  {
    case LL_RCC_SAI1_CLKSOURCE_PCLK2:
    case LL_RCC_SAI2_CLKSOURCE_PCLK2:
      sai_frequency = RCCEx_GetPCLK2Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_SAI1_CLKSOURCE_CLKP:
    case LL_RCC_SAI2_CLKSOURCE_CLKP:
      sai_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_SAI1_CLKSOURCE_IC7:
    case LL_RCC_SAI2_CLKSOURCE_IC7:
      if (LL_RCC_IC7_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC7_GetDivider();
        switch (LL_RCC_IC7_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            sai_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            sai_frequency = sai_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            sai_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            sai_frequency = sai_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            sai_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            sai_frequency = sai_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            sai_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            sai_frequency = sai_frequency / ic_divider;
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
            sai_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            sai_frequency = sai_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            sai_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            sai_frequency = sai_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            sai_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            sai_frequency = sai_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            sai_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            sai_frequency = sai_frequency / ic_divider;
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
        sai_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_SAI1_CLKSOURCE_MSI:
    case LL_RCC_SAI2_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        sai_frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_SAI1_CLKSOURCE_I2S_CKIN:
    case LL_RCC_SAI2_CLKSOURCE_I2S_CKIN:
      sai_frequency = EXTERNAL_CLOCK_VALUE;
      break;

    case LL_RCC_SAI1_CLKSOURCE_SPDIFRX1:
    case LL_RCC_SAI2_CLKSOURCE_SPDIFRX1:
      sai_frequency = RCCEx_GetSPDIFRXCLKFreq(LL_RCC_SPDIFRX1_CLKSOURCE);
      break;

    default:
      /* Unexpected case */
      break;
  }

  return sai_frequency;
}

/**
  * @brief  Return SDMMC clock frequency
  * @param  SDMMCxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_SDMMC1_Clock_Source
  *         @arg @ref RCCEx_SDMMC2_Clock_Source
  * @retval SDMMC clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetSDMMCCLKFreq(uint32_t SDMMCxSource)
{
  uint32_t sdmmc_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetSDMMCClockSource(SDMMCxSource))
  {
    case LL_RCC_SDMMC1_CLKSOURCE_HCLK:
    case LL_RCC_SDMMC2_CLKSOURCE_HCLK:
      sdmmc_frequency = RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq());
      break;

    case LL_RCC_SDMMC1_CLKSOURCE_CLKP:
    case LL_RCC_SDMMC2_CLKSOURCE_CLKP:
      sdmmc_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_SDMMC1_CLKSOURCE_IC4:
    case LL_RCC_SDMMC2_CLKSOURCE_IC4:
      if (LL_RCC_IC4_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC4_GetDivider();
        switch (LL_RCC_IC4_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            sdmmc_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            sdmmc_frequency = sdmmc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            sdmmc_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            sdmmc_frequency = sdmmc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            sdmmc_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            sdmmc_frequency = sdmmc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            sdmmc_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            sdmmc_frequency = sdmmc_frequency / ic_divider;
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
            sdmmc_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            sdmmc_frequency = sdmmc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            sdmmc_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            sdmmc_frequency = sdmmc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            sdmmc_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            sdmmc_frequency = sdmmc_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            sdmmc_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            sdmmc_frequency = sdmmc_frequency / ic_divider;
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

  return sdmmc_frequency;
}

/**
  * @brief  Return SPDIFRX clock frequency
  * @param  SPDIFRXxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_SPDIFRX1_Clock_Source
  * @retval SPDIF clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetSPDIFRXCLKFreq(uint32_t SPDIFRXxSource)
{
  uint32_t spdifrx_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetSPDIFRXClockSource(SPDIFRXxSource))
  {
    case LL_RCC_SPDIFRX1_CLKSOURCE_PCLK1:
      spdifrx_frequency = RCCEx_GetPCLK1Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_SPDIFRX1_CLKSOURCE_CLKP:
      spdifrx_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;

    case LL_RCC_SPDIFRX1_CLKSOURCE_IC7:
      if (LL_RCC_IC7_IsEnabled() != 0U)
      {
        ic_divider = LL_RCC_IC7_GetDivider();
        switch (LL_RCC_IC7_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            spdifrx_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            spdifrx_frequency = spdifrx_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            spdifrx_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            spdifrx_frequency = spdifrx_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            spdifrx_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            spdifrx_frequency = spdifrx_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            spdifrx_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            spdifrx_frequency = spdifrx_frequency / ic_divider;
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
            spdifrx_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            spdifrx_frequency = spdifrx_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            spdifrx_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            spdifrx_frequency = spdifrx_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            spdifrx_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            spdifrx_frequency = spdifrx_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            spdifrx_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            spdifrx_frequency = spdifrx_frequency / ic_divider;
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
        spdifrx_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_SPDIFRX1_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        spdifrx_frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_SPDIFRX1_CLKSOURCE_I2S_CKIN:
      spdifrx_frequency = EXTERNAL_CLOCK_VALUE;
      break;

    default:
      /* Unexpected case */
      break;
  }

  return spdifrx_frequency;
}

/**
  * @brief  Return SPIx clock frequency
  * @param  SPIxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_SPI1_Clock_Source
  *         @arg @ref RCCEx_SPI2_Clock_Source
  *         @arg @ref RCCEx_SPI3_Clock_Source
  *         @arg @ref RCCEx_SPI4_Clock_Source
  *         @arg @ref RCCEx_SPI5_Clock_Source
  *         @arg @ref RCCEx_SPI6_Clock_Source
  * @retval SPI clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetSPICLKFreq(uint32_t SPIxSource)
{
  uint32_t spi_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetSPIClockSource(SPIxSource))
  {
    case LL_RCC_SPI2_CLKSOURCE_PCLK1:
    case LL_RCC_SPI3_CLKSOURCE_PCLK1:
      spi_frequency = RCCEx_GetPCLK1Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_SPI1_CLKSOURCE_PCLK2:
    case LL_RCC_SPI4_CLKSOURCE_PCLK2:
    case LL_RCC_SPI5_CLKSOURCE_PCLK2:
      spi_frequency = RCCEx_GetPCLK2Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_SPI6_CLKSOURCE_PCLK4:
      spi_frequency = RCCEx_GetPCLK4Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_SPI1_CLKSOURCE_CLKP:
    case LL_RCC_SPI2_CLKSOURCE_CLKP:
    case LL_RCC_SPI3_CLKSOURCE_CLKP:
    case LL_RCC_SPI4_CLKSOURCE_CLKP:
    case LL_RCC_SPI5_CLKSOURCE_CLKP:
    case LL_RCC_SPI6_CLKSOURCE_CLKP:
      spi_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
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
            spi_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            spi_frequency = spi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            spi_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            spi_frequency = spi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            spi_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            spi_frequency = spi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            spi_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            spi_frequency = spi_frequency / ic_divider;
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
            spi_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            spi_frequency = spi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            spi_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            spi_frequency = spi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            spi_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            spi_frequency = spi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            spi_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            spi_frequency = spi_frequency / ic_divider;
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
            spi_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            spi_frequency = spi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            spi_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            spi_frequency = spi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            spi_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            spi_frequency = spi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            spi_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            spi_frequency = spi_frequency / ic_divider;
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
        spi_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
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
        spi_frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_SPI1_CLKSOURCE_I2S_CKIN:
    case LL_RCC_SPI2_CLKSOURCE_I2S_CKIN:
    case LL_RCC_SPI3_CLKSOURCE_I2S_CKIN:
    case LL_RCC_SPI6_CLKSOURCE_I2S_CKIN:
      spi_frequency = EXTERNAL_CLOCK_VALUE;
      break;

    case LL_RCC_SPI4_CLKSOURCE_HSE:
    case LL_RCC_SPI5_CLKSOURCE_HSE:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        spi_frequency = HSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return spi_frequency;
}

/**
  * @brief  Return UARTx clock frequency
  * @param  UARTxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_UART4_Clock_Source
  *         @arg @ref RCCEx_UART5_Clock_Source
  *         @arg @ref RCCEx_UART7_Clock_Source
  *         @arg @ref RCCEx_UART8_Clock_Source
  *         @arg @ref RCCEx_UART9_Clock_Source
  * @retval USART clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetUARTCLKFreq(uint32_t UARTxSource)
{
  uint32_t uart_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetUARTClockSource(UARTxSource))
  {
    case LL_RCC_UART4_CLKSOURCE_PCLK1:
    case LL_RCC_UART5_CLKSOURCE_PCLK1:
    case LL_RCC_UART7_CLKSOURCE_PCLK1:
    case LL_RCC_UART8_CLKSOURCE_PCLK1:
      uart_frequency = RCCEx_GetPCLK1Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_UART9_CLKSOURCE_PCLK2:
      uart_frequency = RCCEx_GetPCLK2Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_UART4_CLKSOURCE_CLKP:
    case LL_RCC_UART5_CLKSOURCE_CLKP:
    case LL_RCC_UART7_CLKSOURCE_CLKP:
    case LL_RCC_UART8_CLKSOURCE_CLKP:
    case LL_RCC_UART9_CLKSOURCE_CLKP:
      uart_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
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
            uart_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            uart_frequency = uart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            uart_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            uart_frequency = uart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            uart_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            uart_frequency = uart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            uart_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            uart_frequency = uart_frequency / ic_divider;
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
            uart_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            uart_frequency = uart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            uart_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            uart_frequency = uart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            uart_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            uart_frequency = uart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            uart_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            uart_frequency = uart_frequency / ic_divider;
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
        uart_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_UART4_CLKSOURCE_MSI:
    case LL_RCC_UART5_CLKSOURCE_MSI:
    case LL_RCC_UART7_CLKSOURCE_MSI:
    case LL_RCC_UART8_CLKSOURCE_MSI:
    case LL_RCC_UART9_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        uart_frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_UART4_CLKSOURCE_LSE:
    case LL_RCC_UART5_CLKSOURCE_LSE:
    case LL_RCC_UART7_CLKSOURCE_LSE:
    case LL_RCC_UART8_CLKSOURCE_LSE:
    case LL_RCC_UART9_CLKSOURCE_LSE:
      if (LL_RCC_LSE_IsReady() != 0U)
      {
        uart_frequency = LSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return uart_frequency;
}

/**
  * @brief  Return USARTx clock frequency
  * @param  USARTxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_USART1_Clock_Source
  *         @arg @ref RCCEx_USART2_Clock_Source
  *         @arg @ref RCCEx_USART3_Clock_Source
  *         @arg @ref RCCEx_USART6_Clock_Source
  *         @arg @ref RCCEx_USART10_Clock_Source
  * @retval USART clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */
static uint32_t RCCEx_GetUSARTCLKFreq(uint32_t USARTxSource)
{
  uint32_t usart_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetUSARTClockSource(USARTxSource))
  {
    case LL_RCC_USART1_CLKSOURCE_PCLK2:
    case LL_RCC_USART6_CLKSOURCE_PCLK2:
    case LL_RCC_USART10_CLKSOURCE_PCLK2:
      usart_frequency = RCCEx_GetPCLK2Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_USART2_CLKSOURCE_PCLK1:
    case LL_RCC_USART3_CLKSOURCE_PCLK1:
      usart_frequency = RCCEx_GetPCLK1Freq(RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq()));
      break;

    case LL_RCC_USART1_CLKSOURCE_CLKP:
    case LL_RCC_USART2_CLKSOURCE_CLKP:
    case LL_RCC_USART3_CLKSOURCE_CLKP:
    case LL_RCC_USART6_CLKSOURCE_CLKP:
    case LL_RCC_USART10_CLKSOURCE_CLKP:
      usart_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
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
            usart_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            usart_frequency = usart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            usart_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            usart_frequency = usart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            usart_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            usart_frequency = usart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            usart_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            usart_frequency = usart_frequency / ic_divider;
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
            usart_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            usart_frequency = usart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            usart_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            usart_frequency = usart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            usart_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            usart_frequency = usart_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            usart_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            usart_frequency = usart_frequency / ic_divider;
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
        usart_frequency = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos);
      }
      break;

    case LL_RCC_USART1_CLKSOURCE_MSI:
    case LL_RCC_USART2_CLKSOURCE_MSI:
    case LL_RCC_USART3_CLKSOURCE_MSI:
    case LL_RCC_USART6_CLKSOURCE_MSI:
    case LL_RCC_USART10_CLKSOURCE_MSI:
      if (LL_RCC_MSI_IsReady() != 0U)
      {
        usart_frequency = MSI_VALUE;
      }
      break;

    case LL_RCC_USART1_CLKSOURCE_LSE:
    case LL_RCC_USART2_CLKSOURCE_LSE:
    case LL_RCC_USART3_CLKSOURCE_LSE:
    case LL_RCC_USART6_CLKSOURCE_LSE:
    case LL_RCC_USART10_CLKSOURCE_LSE:
      if (LL_RCC_LSE_IsReady() != 0U)
      {
        usart_frequency = LSE_VALUE;
      }
      break;

    default:
      /* Unexpected case */
      break;
  }

  return usart_frequency;
}

/**
  * @brief  Return OTGPHYx clock frequency
  * @param  OTGPHYxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_USB_OTGHS1_Clock_Source
  *         @arg @ref RCCEx_USB_OTGHS2_Clock_Source
  * @retval OTGPHY clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready or Disabled
  */
static uint32_t RCCEx_GetOTGPHYCLKFreq(uint32_t OTGPHYxSource)
{
  uint32_t usb_frequency = RCC_PERIPH_FREQUENCY_NO;

  switch (LL_RCC_GetUSBClockSource(OTGPHYxSource))
  {
    case LL_RCC_OTGPHY1_CLKSOURCE_CLKP:
    case LL_RCC_OTGPHY2_CLKSOURCE_CLKP:
      usb_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
      break;
    case LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2:
    case LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        usb_frequency = HSE_VALUE / 2U;
      }
      break;
    case LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC:
    case LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2_OSC:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        if (LL_RCC_HSE_IsSelectedHSEDiv2AsDiv2Clock() == 0UL)
        {
          usb_frequency = HSE_VALUE;
        }
        else
        {
          usb_frequency = HSE_VALUE / 2U;
        }
      }
      break;
    case LL_RCC_OTGPHY1_CLKSOURCE_IC15:
    case LL_RCC_OTGPHY2_CLKSOURCE_IC15:
      if (LL_RCC_IC15_IsEnabled() != 0U)
      {
        uint32_t ic_divider = LL_RCC_IC15_GetDivider();
        switch (LL_RCC_IC15_GetSource())
        {
          case LL_RCC_ICCLKSOURCE_PLL1:
            usb_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            usb_frequency = usb_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            usb_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            usb_frequency = usb_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            usb_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            usb_frequency = usb_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            usb_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            usb_frequency = usb_frequency / ic_divider;
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

  return usb_frequency;
}

/**
  * @brief  Return OTGPHYxCKREF clock frequency
  * @param  OTGPHYxCKREFSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_USBPHY1_Clock_Source
  *         @arg @ref RCCEx_USBPHY2_Clock_Source
  * @retval OTGPHYCKREF clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready or Disabled
  */
static uint32_t RCCEx_GetOTGPHYCKREFCLKFreq(uint32_t OTGPHYxCKREFSource)
{
  uint32_t usb_frequency = RCC_PERIPH_FREQUENCY_NO;

  switch (LL_RCC_GetUSBClockSource(OTGPHYxCKREFSource))
  {
    case LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC:
    case LL_RCC_OTGPHY2CKREF_CLKSOURCE_HSE_DIV_2_OSC:
      if (LL_RCC_HSE_IsReady() != 0U)
      {
        if (LL_RCC_HSE_IsSelectedHSEDiv2AsDiv2Clock() == 0UL)
        {
          usb_frequency = HSE_VALUE;
        }
        else
        {
          usb_frequency = HSE_VALUE / 2U;
        }
      }
      break;

    case LL_RCC_OTGPHY1CKREF_CLKSOURCE_OTGPHY1:
      usb_frequency = RCCEx_GetOTGPHYCLKFreq(LL_RCC_OTGPHY1_CLKSOURCE);
      break;

    case LL_RCC_OTGPHY2CKREF_CLKSOURCE_OTGPHY2:
      usb_frequency = RCCEx_GetOTGPHYCLKFreq(LL_RCC_OTGPHY2_CLKSOURCE);
      break;

    default:
      /* Unexpected case */
      break;
  }

  return usb_frequency;
}

/**
  * @brief  Return XSPI clock frequency
  * @param  XSPIxSource This parameter can be one of the following values:
  *         @arg @ref RCCEx_XSPI1_Clock_Source
  *         @arg @ref RCCEx_XSPI2_Clock_Source
  *         @arg @ref RCCEx_XSPI3_Clock_Source
  * @retval XSPI clock frequency (in Hz)
  *         - @ref  RCC_PERIPH_FREQUENCY_NO indicates that oscillator is not ready
  */

static uint32_t RCCEx_GetXSPICLKFreq(uint32_t XSPIxSource)
{
  uint32_t xspi_frequency = RCC_PERIPH_FREQUENCY_NO;
  uint32_t ic_divider;

  switch (LL_RCC_GetXSPIClockSource(XSPIxSource))
  {
    case LL_RCC_XSPI1_CLKSOURCE_HCLK:
    case LL_RCC_XSPI2_CLKSOURCE_HCLK:
    case LL_RCC_XSPI3_CLKSOURCE_HCLK:
      xspi_frequency = RCCEx_GetHCLKFreq(HAL_RCC_GetSysClockFreq());
      break;

    case LL_RCC_XSPI1_CLKSOURCE_CLKP:
    case LL_RCC_XSPI2_CLKSOURCE_CLKP:
    case LL_RCC_XSPI3_CLKSOURCE_CLKP:
      xspi_frequency = RCCEx_GetCLKPCLKFreq(LL_RCC_CLKP_CLKSOURCE);
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
            xspi_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            xspi_frequency = xspi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            xspi_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            xspi_frequency = xspi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            xspi_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            xspi_frequency = xspi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            xspi_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            xspi_frequency = xspi_frequency / ic_divider;
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
            xspi_frequency = HAL_RCCEx_GetPLL1CLKFreq();
            xspi_frequency = xspi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL2:
            xspi_frequency = HAL_RCCEx_GetPLL2CLKFreq();
            xspi_frequency = xspi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL3:
            xspi_frequency = HAL_RCCEx_GetPLL3CLKFreq();
            xspi_frequency = xspi_frequency / ic_divider;
            break;
          case LL_RCC_ICCLKSOURCE_PLL4:
            xspi_frequency = HAL_RCCEx_GetPLL4CLKFreq();
            xspi_frequency = xspi_frequency / ic_divider;
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

  return xspi_frequency;
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
