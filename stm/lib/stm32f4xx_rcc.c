/**
  ******************************************************************************
  * @file    stm32f4xx_rcc.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    08-November-2013
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Reset and clock control (RCC) peripheral:
  *           + Internal/external clocks, PLL, CSS and MCO configuration
  *           + System, AHB and APB busses clocks configuration
  *           + Peripheral clocks configuration
  *           + Interrupts and flags management
  *
 @verbatim                
 ===============================================================================
                      ##### RCC specific features #####
 ===============================================================================
    [..]  
      After reset the device is running from Internal High Speed oscillator 
      (HSI 16MHz) with Flash 0 wait state, Flash prefetch buffer, D-Cache 
      and I-Cache are disabled, and all peripherals are off except internal
      SRAM, Flash and JTAG.
      (+) There is no prescaler on High speed (AHB) and Low speed (APB) busses;
          all peripherals mapped on these busses are running at HSI speed.
      (+) The clock for all peripherals is switched off, except the SRAM and FLASH.
      (+) All GPIOs are in input floating state, except the JTAG pins which
          are assigned to be used for debug purpose.
    [..]          
      Once the device started from reset, the user application has to:        
      (+) Configure the clock source to be used to drive the System clock
          (if the application needs higher frequency/performance)
      (+) Configure the System clock frequency and Flash settings  
      (+) Configure the AHB and APB busses prescalers
      (+) Enable the clock for the peripheral(s) to be used
      (+) Configure the clock source(s) for peripherals which clocks are not
          derived from the System clock (I2S, RTC, ADC, USB OTG FS/SDIO/RNG)                                
 @endverbatim    
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup RCC 
  * @brief RCC driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* ------------ RCC registers bit address in the alias region ----------- */
#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)
/* --- CR Register ---*/
/* Alias word address of HSION bit */
#define CR_OFFSET                 (RCC_OFFSET + 0x00)
#define HSION_BitNumber           0x00
#define CR_HSION_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (HSION_BitNumber * 4))
/* Alias word address of CSSON bit */
#define CSSON_BitNumber           0x13
#define CR_CSSON_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (CSSON_BitNumber * 4))
/* Alias word address of PLLON bit */
#define PLLON_BitNumber           0x18
#define CR_PLLON_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLON_BitNumber * 4))
/* Alias word address of PLLI2SON bit */
#define PLLI2SON_BitNumber        0x1A
#define CR_PLLI2SON_BB            (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLI2SON_BitNumber * 4))

/* Alias word address of PLLSAION bit */
#define PLLSAION_BitNumber        0x1C
#define CR_PLLSAION_BB            (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLSAION_BitNumber * 4))

/* --- CFGR Register ---*/
/* Alias word address of I2SSRC bit */
#define CFGR_OFFSET               (RCC_OFFSET + 0x08)
#define I2SSRC_BitNumber          0x17
#define CFGR_I2SSRC_BB            (PERIPH_BB_BASE + (CFGR_OFFSET * 32) + (I2SSRC_BitNumber * 4))

/* --- BDCR Register ---*/
/* Alias word address of RTCEN bit */
#define BDCR_OFFSET               (RCC_OFFSET + 0x70)
#define RTCEN_BitNumber           0x0F
#define BDCR_RTCEN_BB             (PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (RTCEN_BitNumber * 4))
/* Alias word address of BDRST bit */
#define BDRST_BitNumber           0x10
#define BDCR_BDRST_BB             (PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (BDRST_BitNumber * 4))

/* --- CSR Register ---*/
/* Alias word address of LSION bit */
#define CSR_OFFSET                (RCC_OFFSET + 0x74)
#define LSION_BitNumber           0x00
#define CSR_LSION_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (LSION_BitNumber * 4))

/* --- DCKCFGR Register ---*/
/* Alias word address of TIMPRE bit */
#define DCKCFGR_OFFSET            (RCC_OFFSET + 0x8C)
#define TIMPRE_BitNumber          0x18
#define DCKCFGR_TIMPRE_BB         (PERIPH_BB_BASE + (DCKCFGR_OFFSET * 32) + (TIMPRE_BitNumber * 4))
/* ---------------------- RCC registers bit mask ------------------------ */
/* CFGR register bit mask */
#define CFGR_MCO2_RESET_MASK      ((uint32_t)0x07FFFFFF)
#define CFGR_MCO1_RESET_MASK      ((uint32_t)0xF89FFFFF)

/* RCC Flag Mask */
#define FLAG_MASK                 ((uint8_t)0x1F)

/* CR register byte 3 (Bits[23:16]) base address */
#define CR_BYTE3_ADDRESS          ((uint32_t)0x40023802)

/* CIR register byte 2 (Bits[15:8]) base address */
#define CIR_BYTE2_ADDRESS         ((uint32_t)(RCC_BASE + 0x0C + 0x01))

/* CIR register byte 3 (Bits[23:16]) base address */
#define CIR_BYTE3_ADDRESS         ((uint32_t)(RCC_BASE + 0x0C + 0x02))

/* BDCR register base address */
#define BDCR_ADDRESS              (PERIPH_BASE + BDCR_OFFSET)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __I uint8_t APBAHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup RCC_Private_Functions
  * @{
  */ 

/** @defgroup RCC_Group1 Internal and external clocks, PLL, CSS and MCO configuration functions
 *  @brief   Internal and external clocks, PLL, CSS and MCO configuration functions 
 *
@verbatim   
 ===================================================================================
 ##### Internal and  external clocks, PLL, CSS and MCO configuration functions #####
 ===================================================================================  
    [..]
      This section provide functions allowing to configure the internal/external clocks,
      PLLs, CSS and MCO pins.
  
      (#) HSI (high-speed internal), 16 MHz factory-trimmed RC used directly or through
          the PLL as System clock source.

      (#) LSI (low-speed internal), 32 KHz low consumption RC used as IWDG and/or RTC
          clock source.

      (#) HSE (high-speed external), 4 to 26 MHz crystal oscillator used directly or
          through the PLL as System clock source. Can be used also as RTC clock source.

      (#) LSE (low-speed external), 32 KHz oscillator used as RTC clock source.   

      (#) PLL (clocked by HSI or HSE), featuring two different output clocks:
        (++) The first output is used to generate the high speed system clock (up to 168 MHz)
        (++) The second output is used to generate the clock for the USB OTG FS (48 MHz),
             the random analog generator (<=48 MHz) and the SDIO (<= 48 MHz).

      (#) PLLI2S (clocked by HSI or HSE), used to generate an accurate clock to achieve 
          high-quality audio performance on the I2S interface or SAI interface in case 
          of STM32F429x/439x devices.
     
      (#) PLLSAI clocked by (HSI or HSE), used to generate an accurate clock to SAI 
          interface and LCD TFT controller available only for STM32F42xxx/43xxx devices.
  
      (#) CSS (Clock security system), once enable and if a HSE clock failure occurs 
         (HSE used directly or through PLL as System clock source), the System clock
         is automatically switched to HSI and an interrupt is generated if enabled. 
         The interrupt is linked to the Cortex-M4 NMI (Non-Maskable Interrupt) 
         exception vector.   

      (#) MCO1 (microcontroller clock output), used to output HSI, LSE, HSE or PLL
          clock (through a configurable prescaler) on PA8 pin.

      (#) MCO2 (microcontroller clock output), used to output HSE, PLL, SYSCLK or PLLI2S
          clock (through a configurable prescaler) on PC9 pin.
 @endverbatim
  * @{
  */

/**
  * @brief  Resets the RCC clock configuration to the default reset state.
  * @note   The default reset state of the clock configuration is given below:
  *            - HSI ON and used as system clock source
  *            - HSE, PLL and PLLI2S OFF
  *            - AHB, APB1 and APB2 prescaler set to 1.
  *            - CSS, MCO1 and MCO2 OFF
  *            - All interrupts disabled
  * @note   This function doesn't modify the configuration of the
  *            - Peripheral clocks  
  *            - LSI, LSE and RTC clocks 
  * @param  None
  * @retval None
  */
void RCC_DeInit(void)
{
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON, PLLON, PLLI2S and PLLSAI(STM32F42/43xxx devices) bits */
  RCC->CR &= (uint32_t)0xEAF6FFFF;

  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x24003010;

  /* Reset PLLI2SCFGR register */
  RCC->PLLI2SCFGR = 0x20003000;

  /* Reset PLLSAICFGR register, only available for STM32F42/43xxx devices */
  RCC->PLLSAICFGR = 0x24003000;
 
  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;

  /* Disable Timers clock prescalers selection, only available for STM32F42/43xxx devices */
  RCC->DCKCFGR = 0x00000000; 

}

/**
  * @brief  Configures the External High Speed oscillator (HSE).
  * @note   After enabling the HSE (RCC_HSE_ON or RCC_HSE_Bypass), the application
  *         software should wait on HSERDY flag to be set indicating that HSE clock
  *         is stable and can be used to clock the PLL and/or system clock.
  * @note   HSE state can not be changed if it is used directly or through the
  *         PLL as system clock. In this case, you have to select another source
  *         of the system clock then change the HSE state (ex. disable it).
  * @note   The HSE is stopped by hardware when entering STOP and STANDBY modes.  
  * @note   This function reset the CSSON bit, so if the Clock security system(CSS)
  *         was previously enabled you have to enable it again after calling this
  *         function.    
  * @param  RCC_HSE: specifies the new state of the HSE.
  *          This parameter can be one of the following values:
  *            @arg RCC_HSE_OFF: turn OFF the HSE oscillator, HSERDY flag goes low after
  *                              6 HSE oscillator clock cycles.
  *            @arg RCC_HSE_ON: turn ON the HSE oscillator
  *            @arg RCC_HSE_Bypass: HSE oscillator bypassed with external clock
  * @retval None
  */
void RCC_HSEConfig(uint8_t RCC_HSE)
{
  /* Check the parameters */
  assert_param(IS_RCC_HSE(RCC_HSE));

  /* Reset HSEON and HSEBYP bits before configuring the HSE ------------------*/
  *(__IO uint8_t *) CR_BYTE3_ADDRESS = RCC_HSE_OFF;

  /* Set the new HSE configuration -------------------------------------------*/
  *(__IO uint8_t *) CR_BYTE3_ADDRESS = RCC_HSE;
}

/**
  * @brief  Waits for HSE start-up.
  * @note   This functions waits on HSERDY flag to be set and return SUCCESS if 
  *         this flag is set, otherwise returns ERROR if the timeout is reached 
  *         and this flag is not set. The timeout value is defined by the constant
  *         HSE_STARTUP_TIMEOUT in stm32f4xx.h file. You can tailor it depending
  *         on the HSE crystal used in your application. 
  * @param  None
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: HSE oscillator is stable and ready to use
  *          - ERROR: HSE oscillator not yet ready
  */
ErrorStatus RCC_WaitForHSEStartUp(void)
{
  __IO uint32_t startupcounter = 0;
  ErrorStatus status = ERROR;
  FlagStatus hsestatus = RESET;
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    hsestatus = RCC_GetFlagStatus(RCC_FLAG_HSERDY);
    startupcounter++;
  } while((startupcounter != HSE_STARTUP_TIMEOUT) && (hsestatus == RESET));

  if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET)
  {
    status = SUCCESS;
  }
  else
  {
    status = ERROR;
  }
  return (status);
}

/**
  * @brief  Adjusts the Internal High Speed oscillator (HSI) calibration value.
  * @note   The calibration is used to compensate for the variations in voltage
  *         and temperature that influence the frequency of the internal HSI RC.
  * @param  HSICalibrationValue: specifies the calibration trimming value.
  *         This parameter must be a number between 0 and 0x1F.
  * @retval None
  */
void RCC_AdjustHSICalibrationValue(uint8_t HSICalibrationValue)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_RCC_CALIBRATION_VALUE(HSICalibrationValue));

  tmpreg = RCC->CR;

  /* Clear HSITRIM[4:0] bits */
  tmpreg &= ~RCC_CR_HSITRIM;

  /* Set the HSITRIM[4:0] bits according to HSICalibrationValue value */
  tmpreg |= (uint32_t)HSICalibrationValue << 3;

  /* Store the new value */
  RCC->CR = tmpreg;
}

/**
  * @brief  Enables or disables the Internal High Speed oscillator (HSI).
  * @note   The HSI is stopped by hardware when entering STOP and STANDBY modes.
  *         It is used (enabled by hardware) as system clock source after startup
  *         from Reset, wakeup from STOP and STANDBY mode, or in case of failure
  *         of the HSE used directly or indirectly as system clock (if the Clock
  *         Security System CSS is enabled).             
  * @note   HSI can not be stopped if it is used as system clock source. In this case,
  *         you have to select another source of the system clock then stop the HSI.  
  * @note   After enabling the HSI, the application software should wait on HSIRDY
  *         flag to be set indicating that HSI clock is stable and can be used as
  *         system clock source.  
  * @param  NewState: new state of the HSI.
  *          This parameter can be: ENABLE or DISABLE.
  * @note   When the HSI is stopped, HSIRDY flag goes low after 6 HSI oscillator
  *         clock cycles.  
  * @retval None
  */
void RCC_HSICmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  *(__IO uint32_t *) CR_HSION_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the External Low Speed oscillator (LSE).
  * @note   As the LSE is in the Backup domain and write access is denied to
  *         this domain after reset, you have to enable write access using 
  *         PWR_BackupAccessCmd(ENABLE) function before to configure the LSE
  *         (to be done once after reset).  
  * @note   After enabling the LSE (RCC_LSE_ON or RCC_LSE_Bypass), the application
  *         software should wait on LSERDY flag to be set indicating that LSE clock
  *         is stable and can be used to clock the RTC.
  * @param  RCC_LSE: specifies the new state of the LSE.
  *          This parameter can be one of the following values:
  *            @arg RCC_LSE_OFF: turn OFF the LSE oscillator, LSERDY flag goes low after
  *                              6 LSE oscillator clock cycles.
  *            @arg RCC_LSE_ON: turn ON the LSE oscillator
  *            @arg RCC_LSE_Bypass: LSE oscillator bypassed with external clock
  * @retval None
  */
void RCC_LSEConfig(uint8_t RCC_LSE)
{
  /* Check the parameters */
  assert_param(IS_RCC_LSE(RCC_LSE));

  /* Reset LSEON and LSEBYP bits before configuring the LSE ------------------*/
  /* Reset LSEON bit */
  *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_OFF;

  /* Reset LSEBYP bit */
  *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_OFF;

  /* Configure LSE (RCC_LSE_OFF is already covered by the code section above) */
  switch (RCC_LSE)
  {
    case RCC_LSE_ON:
      /* Set LSEON bit */
      *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_ON;
      break;
    case RCC_LSE_Bypass:
      /* Set LSEBYP and LSEON bits */
      *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_Bypass | RCC_LSE_ON;
      break;
    default:
      break;
  }
}

/**
  * @brief  Enables or disables the Internal Low Speed oscillator (LSI).
  * @note   After enabling the LSI, the application software should wait on 
  *         LSIRDY flag to be set indicating that LSI clock is stable and can
  *         be used to clock the IWDG and/or the RTC.
  * @note   LSI can not be disabled if the IWDG is running.  
  * @param  NewState: new state of the LSI.
  *          This parameter can be: ENABLE or DISABLE.
  * @note   When the LSI is stopped, LSIRDY flag goes low after 6 LSI oscillator
  *         clock cycles. 
  * @retval None
  */
void RCC_LSICmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  *(__IO uint32_t *) CSR_LSION_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the main PLL clock source, multiplication and division factors.
  * @note   This function must be used only when the main PLL is disabled.
  *  
  * @param  RCC_PLLSource: specifies the PLL entry clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_PLLSource_HSI: HSI oscillator clock selected as PLL clock entry
  *            @arg RCC_PLLSource_HSE: HSE oscillator clock selected as PLL clock entry
  * @note   This clock source (RCC_PLLSource) is common for the main PLL and PLLI2S.  
  *  
  * @param  PLLM: specifies the division factor for PLL VCO input clock
  *          This parameter must be a number between 0 and 63.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 2 MHz. It is recommended to select a frequency
  *         of 2 MHz to limit PLL jitter.
  *  
  * @param  PLLN: specifies the multiplication factor for PLL VCO output clock
  *          This parameter must be a number between 192 and 432.
  * @note   You have to set the PLLN parameter correctly to ensure that the VCO
  *         output frequency is between 192 and 432 MHz.
  *   
  * @param  PLLP: specifies the division factor for main system clock (SYSCLK)
  *          This parameter must be a number in the range {2, 4, 6, or 8}.
  * @note   You have to set the PLLP parameter correctly to not exceed 168 MHz on
  *         the System clock frequency.
  *  
  * @param  PLLQ: specifies the division factor for OTG FS, SDIO and RNG clocks
  *          This parameter must be a number between 4 and 15.
  * @note   If the USB OTG FS is used in your application, you have to set the
  *         PLLQ parameter correctly to have 48 MHz clock for the USB. However,
  *         the SDIO and RNG need a frequency lower than or equal to 48 MHz to work
  *         correctly.
  *   
  * @retval None
  */
void RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t PLLM, uint32_t PLLN, uint32_t PLLP, uint32_t PLLQ)
{
  /* Check the parameters */
  assert_param(IS_RCC_PLL_SOURCE(RCC_PLLSource));
  assert_param(IS_RCC_PLLM_VALUE(PLLM));
  assert_param(IS_RCC_PLLN_VALUE(PLLN));
  assert_param(IS_RCC_PLLP_VALUE(PLLP));
  assert_param(IS_RCC_PLLQ_VALUE(PLLQ));

  RCC->PLLCFGR = PLLM | (PLLN << 6) | (((PLLP >> 1) -1) << 16) | (RCC_PLLSource) |
                 (PLLQ << 24);
}

/**
  * @brief  Enables or disables the main PLL.
  * @note   After enabling the main PLL, the application software should wait on 
  *         PLLRDY flag to be set indicating that PLL clock is stable and can
  *         be used as system clock source.
  * @note   The main PLL can not be disabled if it is used as system clock source
  * @note   The main PLL is disabled by hardware when entering STOP and STANDBY modes.
  * @param  NewState: new state of the main PLL. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PLLCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CR_PLLON_BB = (uint32_t)NewState;
}

#if defined (STM32F40_41xxx) || defined (STM32F401xx)
/**
  * @brief  Configures the PLLI2S clock multiplication and division factors.
  *  
  * @note   This function can be used only for STM32F405xx/407xx, STM32F415xx/417xx 
  *         or STM32F401xx devices. 
  *    
  * @note   This function must be used only when the PLLI2S is disabled.
  * @note   PLLI2S clock source is common with the main PLL (configured in 
  *         RCC_PLLConfig function )  
  *             
  * @param  PLLI2SN: specifies the multiplication factor for PLLI2S VCO output clock
  *          This parameter must be a number between 192 and 432.
  * @note   You have to set the PLLI2SN parameter correctly to ensure that the VCO 
  *         output frequency is between 192 and 432 MHz.
  *    
  * @param  PLLI2SR: specifies the division factor for I2S clock
  *          This parameter must be a number between 2 and 7.
  * @note   You have to set the PLLI2SR parameter correctly to not exceed 192 MHz
  *         on the I2S clock frequency.
  *   
  * @retval None
  */
void RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SR)
{
  /* Check the parameters */
  assert_param(IS_RCC_PLLI2SN_VALUE(PLLI2SN));
  assert_param(IS_RCC_PLLI2SR_VALUE(PLLI2SR));

  RCC->PLLI2SCFGR = (PLLI2SN << 6) | (PLLI2SR << 28);
}
#endif /* STM32F40_41xxx || STM32F401xx */

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
/**
  * @brief  Configures the PLLI2S clock multiplication and division factors.
  * 
  * @note   This function can be used only for STM32F42xxx/43xxx devices 
  *         
  * @note   This function must be used only when the PLLI2S is disabled.
  * @note   PLLI2S clock source is common with the main PLL (configured in 
  *         RCC_PLLConfig function )  
  *             
  * @param  PLLI2SN: specifies the multiplication factor for PLLI2S VCO output clock
  *          This parameter must be a number between 192 and 432.
  * @note   You have to set the PLLI2SN parameter correctly to ensure that the VCO 
  *         output frequency is between 192 and 432 MHz.
  * 
  * @param  PLLI2SQ: specifies the division factor for SAI1 clock
  *          This parameter must be a number between 2 and 15.
  *                 
  * @param  PLLI2SR: specifies the division factor for I2S clock
  *          This parameter must be a number between 2 and 7.
  * @note   You have to set the PLLI2SR parameter correctly to not exceed 192 MHz
  *         on the I2S clock frequency.
  * @note   the PLLI2SR parameter is only available with STM32F42xxx/43xxx devices.  
  *   
  * @retval None
  */
void RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SQ, uint32_t PLLI2SR)
{
  /* Check the parameters */
  assert_param(IS_RCC_PLLI2SN_VALUE(PLLI2SN));
  assert_param(IS_RCC_PLLI2SQ_VALUE(PLLI2SQ));
  assert_param(IS_RCC_PLLI2SR_VALUE(PLLI2SR));

  RCC->PLLI2SCFGR = (PLLI2SN << 6) | (PLLI2SQ << 24) | (PLLI2SR << 28);
}
#endif /* STM32F427_437xx ||  STM32F429_439xx */

/**
  * @brief  Enables or disables the PLLI2S. 
  * @note   The PLLI2S is disabled by hardware when entering STOP and STANDBY modes.  
  * @param  NewState: new state of the PLLI2S. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PLLI2SCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CR_PLLI2SON_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the PLLSAI clock multiplication and division factors.
  *
  * @note   This function can be used only for STM32F42xxx/43xxx devices 
  *        
  * @note   This function must be used only when the PLLSAI is disabled.
  * @note   PLLSAI clock source is common with the main PLL (configured in 
  *         RCC_PLLConfig function )  
  *             
  * @param  PLLSAIN: specifies the multiplication factor for PLLSAI VCO output clock
  *          This parameter must be a number between 192 and 432.
  * @note   You have to set the PLLSAIN parameter correctly to ensure that the VCO 
  *         output frequency is between 192 and 432 MHz.
  *           
  * @param  PLLSAIQ: specifies the division factor for SAI1 clock
  *          This parameter must be a number between 2 and 15.
  *            
  * @param  PLLSAIR: specifies the division factor for LTDC clock
  *          This parameter must be a number between 2 and 7.
  *   
  * @retval None
  */
void RCC_PLLSAIConfig(uint32_t PLLSAIN, uint32_t PLLSAIQ, uint32_t PLLSAIR)
{
  /* Check the parameters */
  assert_param(IS_RCC_PLLSAIN_VALUE(PLLSAIN));
  assert_param(IS_RCC_PLLSAIR_VALUE(PLLSAIR));

  RCC->PLLSAICFGR = (PLLSAIN << 6) | (PLLSAIQ << 24) | (PLLSAIR << 28);
}

/**
  * @brief  Enables or disables the PLLSAI. 
  * 
  * @note   This function can be used only for STM32F42xxx/43xxx devices 
  *       
  * @note   The PLLSAI is disabled by hardware when entering STOP and STANDBY modes.  
  * @param  NewState: new state of the PLLSAI. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PLLSAICmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CR_PLLSAION_BB = (uint32_t)NewState;
}

/**
  * @brief  Enables or disables the Clock Security System.
  * @note   If a failure is detected on the HSE oscillator clock, this oscillator
  *         is automatically disabled and an interrupt is generated to inform the
  *         software about the failure (Clock Security System Interrupt, CSSI),
  *         allowing the MCU to perform rescue operations. The CSSI is linked to 
  *         the Cortex-M4 NMI (Non-Maskable Interrupt) exception vector.  
  * @param  NewState: new state of the Clock Security System.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_ClockSecuritySystemCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CR_CSSON_BB = (uint32_t)NewState;
}

/**
  * @brief  Selects the clock source to output on MCO1 pin(PA8).
  * @note   PA8 should be configured in alternate function mode.
  * @param  RCC_MCO1Source: specifies the clock source to output.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCO1Source_HSI: HSI clock selected as MCO1 source
  *            @arg RCC_MCO1Source_LSE: LSE clock selected as MCO1 source
  *            @arg RCC_MCO1Source_HSE: HSE clock selected as MCO1 source
  *            @arg RCC_MCO1Source_PLLCLK: main PLL clock selected as MCO1 source
  * @param  RCC_MCO1Div: specifies the MCO1 prescaler.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCO1Div_1: no division applied to MCO1 clock
  *            @arg RCC_MCO1Div_2: division by 2 applied to MCO1 clock
  *            @arg RCC_MCO1Div_3: division by 3 applied to MCO1 clock
  *            @arg RCC_MCO1Div_4: division by 4 applied to MCO1 clock
  *            @arg RCC_MCO1Div_5: division by 5 applied to MCO1 clock
  * @retval None
  */
void RCC_MCO1Config(uint32_t RCC_MCO1Source, uint32_t RCC_MCO1Div)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_MCO1SOURCE(RCC_MCO1Source));
  assert_param(IS_RCC_MCO1DIV(RCC_MCO1Div));  

  tmpreg = RCC->CFGR;

  /* Clear MCO1[1:0] and MCO1PRE[2:0] bits */
  tmpreg &= CFGR_MCO1_RESET_MASK;

  /* Select MCO1 clock source and prescaler */
  tmpreg |= RCC_MCO1Source | RCC_MCO1Div;

  /* Store the new value */
  RCC->CFGR = tmpreg;  
}

/**
  * @brief  Selects the clock source to output on MCO2 pin(PC9).
  * @note   PC9 should be configured in alternate function mode.
  * @param  RCC_MCO2Source: specifies the clock source to output.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCO2Source_SYSCLK: System clock (SYSCLK) selected as MCO2 source
  *            @arg RCC_MCO2Source_PLLI2SCLK: PLLI2S clock selected as MCO2 source
  *            @arg RCC_MCO2Source_HSE: HSE clock selected as MCO2 source
  *            @arg RCC_MCO2Source_PLLCLK: main PLL clock selected as MCO2 source
  * @param  RCC_MCO2Div: specifies the MCO2 prescaler.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCO2Div_1: no division applied to MCO2 clock
  *            @arg RCC_MCO2Div_2: division by 2 applied to MCO2 clock
  *            @arg RCC_MCO2Div_3: division by 3 applied to MCO2 clock
  *            @arg RCC_MCO2Div_4: division by 4 applied to MCO2 clock
  *            @arg RCC_MCO2Div_5: division by 5 applied to MCO2 clock
  * @retval None
  */
void RCC_MCO2Config(uint32_t RCC_MCO2Source, uint32_t RCC_MCO2Div)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_MCO2SOURCE(RCC_MCO2Source));
  assert_param(IS_RCC_MCO2DIV(RCC_MCO2Div));
  
  tmpreg = RCC->CFGR;
  
  /* Clear MCO2 and MCO2PRE[2:0] bits */
  tmpreg &= CFGR_MCO2_RESET_MASK;

  /* Select MCO2 clock source and prescaler */
  tmpreg |= RCC_MCO2Source | RCC_MCO2Div;

  /* Store the new value */
  RCC->CFGR = tmpreg;  
}

/**
  * @}
  */

/** @defgroup RCC_Group2 System AHB and APB busses clocks configuration functions
 *  @brief   System, AHB and APB busses clocks configuration functions
 *
@verbatim   
 ===============================================================================
      ##### System, AHB and APB busses clocks configuration functions #####
 ===============================================================================  
    [..]
      This section provide functions allowing to configure the System, AHB, APB1 and 
      APB2 busses clocks.
  
      (#) Several clock sources can be used to drive the System clock (SYSCLK): HSI,
          HSE and PLL.
          The AHB clock (HCLK) is derived from System clock through configurable 
          prescaler and used to clock the CPU, memory and peripherals mapped 
          on AHB bus (DMA, GPIO...). APB1 (PCLK1) and APB2 (PCLK2) clocks are derived 
          from AHB clock through configurable prescalers and used to clock 
          the peripherals mapped on these busses. You can use 
          "RCC_GetClocksFreq()" function to retrieve the frequencies of these clocks.  

      -@- All the peripheral clocks are derived from the System clock (SYSCLK) except:
        (+@) I2S: the I2S clock can be derived either from a specific PLL (PLLI2S) or
             from an external clock mapped on the I2S_CKIN pin. 
             You have to use RCC_I2SCLKConfig() function to configure this clock. 
        (+@) RTC: the RTC clock can be derived either from the LSI, LSE or HSE clock
             divided by 2 to 31. You have to use RCC_RTCCLKConfig() and RCC_RTCCLKCmd()
             functions to configure this clock. 
        (+@) USB OTG FS, SDIO and RTC: USB OTG FS require a frequency equal to 48 MHz
             to work correctly, while the SDIO require a frequency equal or lower than
             to 48. This clock is derived of the main PLL through PLLQ divider.
        (+@) IWDG clock which is always the LSI clock.
       
      (#) For STM32F405xx/407xx and STM32F415xx/417xx devices, the maximum frequency 
         of the SYSCLK and HCLK is 168 MHz, PCLK2 84 MHz and PCLK1 42 MHz. Depending 
         on the device voltage range, the maximum frequency should be adapted accordingly:
 +-------------------------------------------------------------------------------------+     
 | Latency       |                HCLK clock frequency (MHz)                           |
 |               |---------------------------------------------------------------------|     
 |               | voltage range  | voltage range  | voltage range   | voltage range   |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   |
 |---------------|----------------|----------------|-----------------|-----------------|              
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24  |0 < HCLK <= 22   |0 < HCLK <= 20   |
 |---------------|----------------|----------------|-----------------|-----------------|   
 |1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44  |20 < HCLK <= 40  | 
 |---------------|----------------|----------------|-----------------|-----------------|   
 |2WS(3CPU cycle)|60 < HCLK <= 90 |48 < HCLK <= 72 |44 < HCLK <= 66  |40 < HCLK <= 60  |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |3WS(4CPU cycle)|90 < HCLK <= 120|72 < HCLK <= 96 |66 < HCLK <= 88  |60 < HCLK <= 80  |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |4WS(5CPU cycle)|120< HCLK <= 150|96 < HCLK <= 120|88 < HCLK <= 110 |80 < HCLK <= 100 |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |5WS(6CPU cycle)|150< HCLK <= 168|120< HCLK <= 144|110 < HCLK <= 132|100 < HCLK <= 120| 
 |---------------|----------------|----------------|-----------------|-----------------| 
 |6WS(7CPU cycle)|      NA        |144< HCLK <= 168|132 < HCLK <= 154|120 < HCLK <= 140| 
 |---------------|----------------|----------------|-----------------|-----------------| 
 |7WS(8CPU cycle)|      NA        |      NA        |154 < HCLK <= 168|140 < HCLK <= 160|
 +---------------|----------------|----------------|-----------------|-----------------+ 
      (#) For STM32F42xxx/43xxx devices, the maximum frequency of the SYSCLK and HCLK is 180 MHz, 
          PCLK2 90 MHz and PCLK1 45 MHz. Depending on the device voltage range, the maximum 
          frequency should be adapted accordingly:
 +-------------------------------------------------------------------------------------+     
 | Latency       |                HCLK clock frequency (MHz)                           |
 |               |---------------------------------------------------------------------|     
 |               | voltage range  | voltage range  | voltage range   | voltage range   |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   |
 |---------------|----------------|----------------|-----------------|-----------------|              
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24  |0 < HCLK <= 22   |0 < HCLK <= 20   |
 |---------------|----------------|----------------|-----------------|-----------------|   
 |1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44  |20 < HCLK <= 40  | 
 |---------------|----------------|----------------|-----------------|-----------------|   
 |2WS(3CPU cycle)|60 < HCLK <= 90 |48 < HCLK <= 72 |44 < HCLK <= 66  |40 < HCLK <= 60  |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |3WS(4CPU cycle)|90 < HCLK <= 120|72 < HCLK <= 96 |66 < HCLK <= 88  |60 < HCLK <= 80  |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |4WS(5CPU cycle)|120< HCLK <= 150|96 < HCLK <= 120|88 < HCLK <= 110 |80 < HCLK <= 100 |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |5WS(6CPU cycle)|120< HCLK <= 180|120< HCLK <= 144|110 < HCLK <= 132|100 < HCLK <= 120| 
 |---------------|----------------|----------------|-----------------|-----------------| 
 |6WS(7CPU cycle)|      NA        |144< HCLK <= 168|132 < HCLK <= 154|120 < HCLK <= 140| 
 |---------------|----------------|----------------|-----------------|-----------------| 
 |7WS(8CPU cycle)|      NA        |168< HCLK <= 180|154 < HCLK <= 176|140 < HCLK <= 160|
 |---------------|----------------|----------------|-----------------|-----------------| 
 |8WS(9CPU cycle)|      NA        |      NA        |176 < HCLK <= 180|160 < HCLK <= 168|
 +-------------------------------------------------------------------------------------+
   
      (#) For STM32F401xx devices, the maximum frequency of the SYSCLK and HCLK is 84 MHz, 
          PCLK2 84 MHz and PCLK1 42 MHz. Depending on the device voltage range, the maximum 
          frequency should be adapted accordingly:
 +-------------------------------------------------------------------------------------+     
 | Latency       |                HCLK clock frequency (MHz)                           |
 |               |---------------------------------------------------------------------|     
 |               | voltage range  | voltage range  | voltage range   | voltage range   |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   |
 |---------------|----------------|----------------|-----------------|-----------------|              
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24  |0 < HCLK <= 22   |0 < HCLK <= 20   |
 |---------------|----------------|----------------|-----------------|-----------------|   
 |1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44  |20 < HCLK <= 40  | 
 |---------------|----------------|----------------|-----------------|-----------------|   
 |2WS(3CPU cycle)|60 < HCLK <= 84 |48 < HCLK <= 72 |44 < HCLK <= 66  |40 < HCLK <= 60  |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |3WS(4CPU cycle)|      NA        |72 < HCLK <= 84 |66 < HCLK <= 84  |60 < HCLK <= 80  |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |4WS(5CPU cycle)|      NA        |      NA        |      NA         |80 < HCLK <= 84  | 
 +-------------------------------------------------------------------------------------+ 
  
      -@- On STM32F405xx/407xx and STM32F415xx/417xx devices: 
           (++) when VOS = '0', the maximum value of fHCLK = 144MHz. 
           (++) when VOS = '1', the maximum value of fHCLK = 168MHz. 
          [..] 
          On STM32F42xxx/43xxx devices:
           (++) when VOS[1:0] = '0x01', the maximum value of fHCLK is 120MHz.
           (++) when VOS[1:0] = '0x10', the maximum value of fHCLK is 144MHz.
           (++) when VOS[1:0] = '0x11', the maximum value of f  is 168MHz 
          [..]  
          On STM32F401x devices:
           (++) when VOS[1:0] = '0x01', the maximum value of fHCLK is 64MHz.
           (++) when VOS[1:0] = '0x10', the maximum value of fHCLK is 84MHz.
           You can use PWR_MainRegulatorModeConfig() function to control VOS bits.

@endverbatim
  * @{
  */

/**
  * @brief  Configures the system clock (SYSCLK).
  * @note   The HSI is used (enabled by hardware) as system clock source after
  *         startup from Reset, wake-up from STOP and STANDBY mode, or in case
  *         of failure of the HSE used directly or indirectly as system clock
  *         (if the Clock Security System CSS is enabled).
  * @note   A switch from one clock source to another occurs only if the target
  *         clock source is ready (clock stable after startup delay or PLL locked). 
  *         If a clock source which is not yet ready is selected, the switch will
  *         occur when the clock source will be ready. 
  *         You can use RCC_GetSYSCLKSource() function to know which clock is
  *         currently used as system clock source. 
  * @param  RCC_SYSCLKSource: specifies the clock source used as system clock.
  *          This parameter can be one of the following values:
  *            @arg RCC_SYSCLKSource_HSI:    HSI selected as system clock source
  *            @arg RCC_SYSCLKSource_HSE:    HSE selected as system clock source
  *            @arg RCC_SYSCLKSource_PLLCLK: PLL selected as system clock source
  * @retval None
  */
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_RCC_SYSCLK_SOURCE(RCC_SYSCLKSource));

  tmpreg = RCC->CFGR;

  /* Clear SW[1:0] bits */
  tmpreg &= ~RCC_CFGR_SW;

  /* Set SW[1:0] bits according to RCC_SYSCLKSource value */
  tmpreg |= RCC_SYSCLKSource;

  /* Store the new value */
  RCC->CFGR = tmpreg;
}

/**
  * @brief  Returns the clock source used as system clock.
  * @param  None
  * @retval The clock source used as system clock. The returned value can be one
  *         of the following:
  *              - 0x00: HSI used as system clock
  *              - 0x04: HSE used as system clock
  *              - 0x08: PLL used as system clock
  */
uint8_t RCC_GetSYSCLKSource(void)
{
  return ((uint8_t)(RCC->CFGR & RCC_CFGR_SWS));
}

/**
  * @brief  Configures the AHB clock (HCLK).
  * @note   Depending on the device voltage range, the software has to set correctly
  *         these bits to ensure that HCLK not exceed the maximum allowed frequency
  *         (for more details refer to section above
  *           "CPU, AHB and APB busses clocks configuration functions")
  * @param  RCC_SYSCLK: defines the AHB clock divider. This clock is derived from 
  *         the system clock (SYSCLK).
  *          This parameter can be one of the following values:
  *            @arg RCC_SYSCLK_Div1: AHB clock = SYSCLK
  *            @arg RCC_SYSCLK_Div2: AHB clock = SYSCLK/2
  *            @arg RCC_SYSCLK_Div4: AHB clock = SYSCLK/4
  *            @arg RCC_SYSCLK_Div8: AHB clock = SYSCLK/8
  *            @arg RCC_SYSCLK_Div16: AHB clock = SYSCLK/16
  *            @arg RCC_SYSCLK_Div64: AHB clock = SYSCLK/64
  *            @arg RCC_SYSCLK_Div128: AHB clock = SYSCLK/128
  *            @arg RCC_SYSCLK_Div256: AHB clock = SYSCLK/256
  *            @arg RCC_SYSCLK_Div512: AHB clock = SYSCLK/512
  * @retval None
  */
void RCC_HCLKConfig(uint32_t RCC_SYSCLK)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_HCLK(RCC_SYSCLK));

  tmpreg = RCC->CFGR;

  /* Clear HPRE[3:0] bits */
  tmpreg &= ~RCC_CFGR_HPRE;

  /* Set HPRE[3:0] bits according to RCC_SYSCLK value */
  tmpreg |= RCC_SYSCLK;

  /* Store the new value */
  RCC->CFGR = tmpreg;
}


/**
  * @brief  Configures the Low Speed APB clock (PCLK1).
  * @param  RCC_HCLK: defines the APB1 clock divider. This clock is derived from 
  *         the AHB clock (HCLK).
  *          This parameter can be one of the following values:
  *            @arg RCC_HCLK_Div1:  APB1 clock = HCLK
  *            @arg RCC_HCLK_Div2:  APB1 clock = HCLK/2
  *            @arg RCC_HCLK_Div4:  APB1 clock = HCLK/4
  *            @arg RCC_HCLK_Div8:  APB1 clock = HCLK/8
  *            @arg RCC_HCLK_Div16: APB1 clock = HCLK/16
  * @retval None
  */
void RCC_PCLK1Config(uint32_t RCC_HCLK)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_RCC_PCLK(RCC_HCLK));

  tmpreg = RCC->CFGR;

  /* Clear PPRE1[2:0] bits */
  tmpreg &= ~RCC_CFGR_PPRE1;

  /* Set PPRE1[2:0] bits according to RCC_HCLK value */
  tmpreg |= RCC_HCLK;

  /* Store the new value */
  RCC->CFGR = tmpreg;
}

/**
  * @brief  Configures the High Speed APB clock (PCLK2).
  * @param  RCC_HCLK: defines the APB2 clock divider. This clock is derived from 
  *         the AHB clock (HCLK).
  *          This parameter can be one of the following values:
  *            @arg RCC_HCLK_Div1:  APB2 clock = HCLK
  *            @arg RCC_HCLK_Div2:  APB2 clock = HCLK/2
  *            @arg RCC_HCLK_Div4:  APB2 clock = HCLK/4
  *            @arg RCC_HCLK_Div8:  APB2 clock = HCLK/8
  *            @arg RCC_HCLK_Div16: APB2 clock = HCLK/16
  * @retval None
  */
void RCC_PCLK2Config(uint32_t RCC_HCLK)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_RCC_PCLK(RCC_HCLK));

  tmpreg = RCC->CFGR;

  /* Clear PPRE2[2:0] bits */
  tmpreg &= ~RCC_CFGR_PPRE2;

  /* Set PPRE2[2:0] bits according to RCC_HCLK value */
  tmpreg |= RCC_HCLK << 3;

  /* Store the new value */
  RCC->CFGR = tmpreg;
}

/**
  * @brief  Returns the frequencies of different on chip clocks; SYSCLK, HCLK, 
  *         PCLK1 and PCLK2.       
  * 
  * @note   The system frequency computed by this function is not the real 
  *         frequency in the chip. It is calculated based on the predefined 
  *         constant and the selected clock source:
  * @note     If SYSCLK source is HSI, function returns values based on HSI_VALUE(*)
  * @note     If SYSCLK source is HSE, function returns values based on HSE_VALUE(**)
  * @note     If SYSCLK source is PLL, function returns values based on HSE_VALUE(**) 
  *           or HSI_VALUE(*) multiplied/divided by the PLL factors.         
  * @note     (*) HSI_VALUE is a constant defined in stm32f4xx.h file (default value
  *               16 MHz) but the real value may vary depending on the variations
  *               in voltage and temperature.
  * @note     (**) HSE_VALUE is a constant defined in stm32f4xx.h file (default value
  *                25 MHz), user has to ensure that HSE_VALUE is same as the real
  *                frequency of the crystal used. Otherwise, this function may
  *                have wrong result.
  *                
  * @note   The result of this function could be not correct when using fractional
  *         value for HSE crystal.
  *   
  * @param  RCC_Clocks: pointer to a RCC_ClocksTypeDef structure which will hold
  *          the clocks frequencies.
  *     
  * @note   This function can be used by the user application to compute the 
  *         baudrate for the communication peripherals or configure other parameters.
  * @note   Each time SYSCLK, HCLK, PCLK1 and/or PCLK2 clock changes, this function
  *         must be called to update the structure's field. Otherwise, any
  *         configuration based on this function will be incorrect.
  *    
  * @retval None
  */
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks)
{
  uint32_t tmp = 0, presc = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp)
  {
    case 0x00:  /* HSI used as system clock source */
      RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
      break;
    case 0x04:  /* HSE used as system clock  source */
      RCC_Clocks->SYSCLK_Frequency = HSE_VALUE;
      break;
    case 0x08:  /* PLL used as system clock  source */

      /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLLM) * PLLN
         SYSCLK = PLL_VCO / PLLP
         */    
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
      pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
      
      if (pllsource != 0)
      {
        /* HSE used as PLL clock source */
        pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }
      else
      {
        /* HSI used as PLL clock source */
        pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);      
      }

      pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
      RCC_Clocks->SYSCLK_Frequency = pllvco/pllp;
      break;
    default:
      RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
      break;
  }
  /* Compute HCLK, PCLK1 and PCLK2 clocks frequencies ------------------------*/

  /* Get HCLK prescaler */
  tmp = RCC->CFGR & RCC_CFGR_HPRE;
  tmp = tmp >> 4;
  presc = APBAHBPrescTable[tmp];
  /* HCLK clock frequency */
  RCC_Clocks->HCLK_Frequency = RCC_Clocks->SYSCLK_Frequency >> presc;

  /* Get PCLK1 prescaler */
  tmp = RCC->CFGR & RCC_CFGR_PPRE1;
  tmp = tmp >> 10;
  presc = APBAHBPrescTable[tmp];
  /* PCLK1 clock frequency */
  RCC_Clocks->PCLK1_Frequency = RCC_Clocks->HCLK_Frequency >> presc;

  /* Get PCLK2 prescaler */
  tmp = RCC->CFGR & RCC_CFGR_PPRE2;
  tmp = tmp >> 13;
  presc = APBAHBPrescTable[tmp];
  /* PCLK2 clock frequency */
  RCC_Clocks->PCLK2_Frequency = RCC_Clocks->HCLK_Frequency >> presc;
}

/**
  * @}
  */

/** @defgroup RCC_Group3 Peripheral clocks configuration functions
 *  @brief   Peripheral clocks configuration functions 
 *
@verbatim   
 ===============================================================================
              ##### Peripheral clocks configuration functions #####
 ===============================================================================  
    [..] This section provide functions allowing to configure the Peripheral clocks. 
  
      (#) The RTC clock which is derived from the LSI, LSE or HSE clock divided 
          by 2 to 31.
     
      (#) After restart from Reset or wakeup from STANDBY, all peripherals are off
          except internal SRAM, Flash and JTAG. Before to start using a peripheral 
          you have to enable its interface clock. You can do this using 
          RCC_AHBPeriphClockCmd(), RCC_APB2PeriphClockCmd() and RCC_APB1PeriphClockCmd() functions.

      (#) To reset the peripherals configuration (to the default state after device reset)
          you can use RCC_AHBPeriphResetCmd(), RCC_APB2PeriphResetCmd() and 
          RCC_APB1PeriphResetCmd() functions.
     
      (#) To further reduce power consumption in SLEEP mode the peripheral clocks 
          can be disabled prior to executing the WFI or WFE instructions. 
          You can do this using RCC_AHBPeriphClockLPModeCmd(), 
          RCC_APB2PeriphClockLPModeCmd() and RCC_APB1PeriphClockLPModeCmd() functions.  

@endverbatim
  * @{
  */

/**
  * @brief  Configures the RTC clock (RTCCLK).
  * @note   As the RTC clock configuration bits are in the Backup domain and write
  *         access is denied to this domain after reset, you have to enable write
  *         access using PWR_BackupAccessCmd(ENABLE) function before to configure
  *         the RTC clock source (to be done once after reset).    
  * @note   Once the RTC clock is configured it can't be changed unless the  
  *         Backup domain is reset using RCC_BackupResetCmd() function, or by
  *         a Power On Reset (POR).
  *    
  * @param  RCC_RTCCLKSource: specifies the RTC clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_RTCCLKSource_LSE: LSE selected as RTC clock
  *            @arg RCC_RTCCLKSource_LSI: LSI selected as RTC clock
  *            @arg RCC_RTCCLKSource_HSE_Divx: HSE clock divided by x selected
  *                                            as RTC clock, where x:[2,31]
  *  
  * @note   If the LSE or LSI is used as RTC clock source, the RTC continues to
  *         work in STOP and STANDBY modes, and can be used as wakeup source.
  *         However, when the HSE clock is used as RTC clock source, the RTC
  *         cannot be used in STOP and STANDBY modes.    
  * @note   The maximum input clock frequency for RTC is 1MHz (when using HSE as
  *         RTC clock source).
  *  
  * @retval None
  */
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_RCC_RTCCLK_SOURCE(RCC_RTCCLKSource));

  if ((RCC_RTCCLKSource & 0x00000300) == 0x00000300)
  { /* If HSE is selected as RTC clock source, configure HSE division factor for RTC clock */
    tmpreg = RCC->CFGR;

    /* Clear RTCPRE[4:0] bits */
    tmpreg &= ~RCC_CFGR_RTCPRE;

    /* Configure HSE division factor for RTC clock */
    tmpreg |= (RCC_RTCCLKSource & 0xFFFFCFF);

    /* Store the new value */
    RCC->CFGR = tmpreg;
  }
    
  /* Select the RTC clock source */
  RCC->BDCR |= (RCC_RTCCLKSource & 0x00000FFF);
}

/**
  * @brief  Enables or disables the RTC clock.
  * @note   This function must be used only after the RTC clock source was selected
  *         using the RCC_RTCCLKConfig function.
  * @param  NewState: new state of the RTC clock. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_RTCCLKCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  *(__IO uint32_t *) BDCR_RTCEN_BB = (uint32_t)NewState;
}

/**
  * @brief  Forces or releases the Backup domain reset.
  * @note   This function resets the RTC peripheral (including the backup registers)
  *         and the RTC clock source selection in RCC_CSR register.
  * @note   The BKPSRAM is not affected by this reset.    
  * @param  NewState: new state of the Backup domain reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_BackupResetCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) BDCR_BDRST_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the I2S clock source (I2SCLK).
  * @note   This function must be called before enabling the I2S APB clock.
  * @param  RCC_I2SCLKSource: specifies the I2S clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2S2CLKSource_PLLI2S: PLLI2S clock used as I2S clock source
  *            @arg RCC_I2S2CLKSource_Ext: External clock mapped on the I2S_CKIN pin
  *                                        used as I2S clock source
  * @retval None
  */
void RCC_I2SCLKConfig(uint32_t RCC_I2SCLKSource)
{
  /* Check the parameters */
  assert_param(IS_RCC_I2SCLK_SOURCE(RCC_I2SCLKSource));

  *(__IO uint32_t *) CFGR_I2SSRC_BB = RCC_I2SCLKSource;
}

/**
  * @brief  Configures the SAI clock Divider coming from PLLI2S.
  * 
  * @note   This function can be used only for STM32F42xxx/43xxx devices.
  *   
  * @note   This function must be called before enabling the PLLI2S.
  *              
  * @param  RCC_PLLI2SDivQ: specifies the PLLI2S division factor for SAI1 clock .
  *          This parameter must be a number between 1 and 32.
  *          SAI1 clock frequency = f(PLLI2S_Q) / RCC_PLLI2SDivQ 
  *              
  * @retval None
  */
void RCC_SAIPLLI2SClkDivConfig(uint32_t RCC_PLLI2SDivQ)  
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_PLLI2S_DIVQ_VALUE(RCC_PLLI2SDivQ));
  
  tmpreg = RCC->DCKCFGR;

  /* Clear PLLI2SDIVQ[4:0] bits */
  tmpreg &= ~(RCC_DCKCFGR_PLLI2SDIVQ);

  /* Set PLLI2SDIVQ values */
  tmpreg |= (RCC_PLLI2SDivQ - 1);

  /* Store the new value */
  RCC->DCKCFGR = tmpreg;
}

/**
  * @brief  Configures the SAI clock Divider coming from PLLSAI.
  * 
  * @note   This function can be used only for STM32F42xxx/43xxx devices.
  *        
  * @note   This function must be called before enabling the PLLSAI.
  *   
  * @param  RCC_PLLSAIDivQ: specifies the PLLSAI division factor for SAI1 clock .
  *          This parameter must be a number between 1 and 32.
  *          SAI1 clock frequency = f(PLLSAI_Q) / RCC_PLLSAIDivQ  
  *              
  * @retval None
  */
void RCC_SAIPLLSAIClkDivConfig(uint32_t RCC_PLLSAIDivQ)  
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_PLLSAI_DIVQ_VALUE(RCC_PLLSAIDivQ));
  
  tmpreg = RCC->DCKCFGR;

  /* Clear PLLI2SDIVQ[4:0] and PLLSAIDIVQ[4:0] bits */
  tmpreg &= ~(RCC_DCKCFGR_PLLSAIDIVQ);

  /* Set PLLSAIDIVQ values */
  tmpreg |= ((RCC_PLLSAIDivQ - 1) << 8);

  /* Store the new value */
  RCC->DCKCFGR = tmpreg;
}

/**
  * @brief  Configures SAI1BlockA clock source selection.
  * 
  * @note   This function can be used only for STM32F42xxx/43xxx devices.
  *       
  * @note   This function must be called before enabling PLLSAI, PLLI2S and  
  *         the SAI clock.
  * @param  RCC_SAIBlockACLKSource: specifies the SAI Block A clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_SAIACLKSource_PLLI2S: PLLI2S_Q clock divided by PLLI2SDIVQ used 
  *                                           as SAI1 Block A clock 
  *            @arg RCC_SAIACLKSource_PLLSAI: PLLISAI_Q clock divided by PLLSAIDIVQ used 
  *                                           as SAI1 Block A clock 
  *            @arg RCC_SAIACLKSource_Ext: External clock mapped on the I2S_CKIN pin
  *                                        used as SAI1 Block A clock
  * @retval None
  */
void RCC_SAIBlockACLKConfig(uint32_t RCC_SAIBlockACLKSource)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_SAIACLK_SOURCE(RCC_SAIBlockACLKSource));
  
  tmpreg = RCC->DCKCFGR;

  /* Clear RCC_DCKCFGR_SAI1ASRC[1:0] bits */
  tmpreg &= ~RCC_DCKCFGR_SAI1ASRC;

  /* Set SAI Block A source selection value */
  tmpreg |= RCC_SAIBlockACLKSource;

  /* Store the new value */
  RCC->DCKCFGR = tmpreg;
}

/**
  * @brief  Configures SAI1BlockB clock source selection.
  * 
  * @note   This function can be used only for STM32F42xxx/43xxx devices.
  *       
  * @note   This function must be called before enabling PLLSAI, PLLI2S and  
  *         the SAI clock.
  * @param  RCC_SAIBlockBCLKSource: specifies the SAI Block B clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_SAIBCLKSource_PLLI2S: PLLI2S_Q clock divided by PLLI2SDIVQ used 
  *                                           as SAI1 Block B clock 
  *            @arg RCC_SAIBCLKSource_PLLSAI: PLLISAI_Q clock divided by PLLSAIDIVQ used 
  *                                           as SAI1 Block B clock 
  *            @arg RCC_SAIBCLKSource_Ext: External clock mapped on the I2S_CKIN pin
  *                                        used as SAI1 Block B clock
  * @retval None
  */
void RCC_SAIBlockBCLKConfig(uint32_t RCC_SAIBlockBCLKSource)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_SAIBCLK_SOURCE(RCC_SAIBlockBCLKSource));
  
  tmpreg = RCC->DCKCFGR;

  /* Clear RCC_DCKCFGR_SAI1BSRC[1:0] bits */
  tmpreg &= ~RCC_DCKCFGR_SAI1BSRC;

  /* Set SAI Block B source selection value */
  tmpreg |= RCC_SAIBlockBCLKSource;

  /* Store the new value */
  RCC->DCKCFGR = tmpreg;
}


/**
  * @brief  Configures the LTDC clock Divider coming from PLLSAI.
  * 
  * @note   The LTDC peripheral is only available with STM32F429xx/439xx Devices.
  *      
  * @note   This function must be called before enabling the PLLSAI.
  *   
  * @param  RCC_PLLSAIDivR: specifies the PLLSAI division factor for LTDC clock .
  *          This parameter must be a number between 2 and 16.
  *          LTDC clock frequency = f(PLLSAI_R) / RCC_PLLSAIDivR  
  *            
  * @retval None
  */
void RCC_LTDCCLKDivConfig(uint32_t RCC_PLLSAIDivR)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_PLLSAI_DIVR_VALUE(RCC_PLLSAIDivR));
  
  tmpreg = RCC->DCKCFGR;

  /* Clear PLLSAIDIVR[2:0] bits */
  tmpreg &= ~RCC_DCKCFGR_PLLSAIDIVR;

  /* Set PLLSAIDIVR values */
  tmpreg |= RCC_PLLSAIDivR;

  /* Store the new value */
  RCC->DCKCFGR = tmpreg;
}

/**
  * @brief  Configures the Timers clocks prescalers selection.
  * 
  * @note   This function can be used only for STM32F42xxx/43xxx and STM32F401xx devices. 
  *   
  * @param  RCC_TIMCLKPrescaler : specifies the Timers clocks prescalers selection
  *         This parameter can be one of the following values:
  *            @arg RCC_TIMPrescDesactivated: The Timers kernels clocks prescaler is 
  *                 equal to HPRE if PPREx is corresponding to division by 1 or 2, 
  *                 else it is equal to [(HPRE * PPREx) / 2] if PPREx is corresponding to 
  *                 division by 4 or more.
  *                   
  *            @arg RCC_TIMPrescActivated: The Timers kernels clocks prescaler is 
  *                 equal to HPRE if PPREx is corresponding to division by 1, 2 or 4, 
  *                 else it is equal to [(HPRE * PPREx) / 4] if PPREx is corresponding 
  *                 to division by 8 or more.
  * @retval None
  */
void RCC_TIMCLKPresConfig(uint32_t RCC_TIMCLKPrescaler)
{
  /* Check the parameters */
  assert_param(IS_RCC_TIMCLK_PRESCALER(RCC_TIMCLKPrescaler));

  *(__IO uint32_t *) DCKCFGR_TIMPRE_BB = RCC_TIMCLKPrescaler;
  
}

/**
  * @brief  Enables or disables the AHB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.   
  * @param  RCC_AHBPeriph: specifies the AHB1 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB1Periph_GPIOA:       GPIOA clock
  *            @arg RCC_AHB1Periph_GPIOB:       GPIOB clock 
  *            @arg RCC_AHB1Periph_GPIOC:       GPIOC clock
  *            @arg RCC_AHB1Periph_GPIOD:       GPIOD clock
  *            @arg RCC_AHB1Periph_GPIOE:       GPIOE clock
  *            @arg RCC_AHB1Periph_GPIOF:       GPIOF clock
  *            @arg RCC_AHB1Periph_GPIOG:       GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOG:       GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOI:       GPIOI clock
  *            @arg RCC_AHB1Periph_GPIOJ:       GPIOJ clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_AHB1Periph_GPIOK:       GPIOK clock (STM32F42xxx/43xxx devices)  
  *            @arg RCC_AHB1Periph_CRC:         CRC clock
  *            @arg RCC_AHB1Periph_BKPSRAM:     BKPSRAM interface clock
  *            @arg RCC_AHB1Periph_CCMDATARAMEN CCM data RAM interface clock
  *            @arg RCC_AHB1Periph_DMA1:        DMA1 clock
  *            @arg RCC_AHB1Periph_DMA2:        DMA2 clock
  *            @arg RCC_AHB1Periph_DMA2D:       DMA2D clock (STM32F429xx/439xx devices)  
  *            @arg RCC_AHB1Periph_ETH_MAC:     Ethernet MAC clock
  *            @arg RCC_AHB1Periph_ETH_MAC_Tx:  Ethernet Transmission clock
  *            @arg RCC_AHB1Periph_ETH_MAC_Rx:  Ethernet Reception clock
  *            @arg RCC_AHB1Periph_ETH_MAC_PTP: Ethernet PTP clock
  *            @arg RCC_AHB1Periph_OTG_HS:      USB OTG HS clock
  *            @arg RCC_AHB1Periph_OTG_HS_ULPI: USB OTG HS ULPI clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB1PeriphClockCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB1_CLOCK_PERIPH(RCC_AHB1Periph));

  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB1ENR |= RCC_AHB1Periph;
  }
  else
  {
    RCC->AHB1ENR &= ~RCC_AHB1Periph;
  }
}

/**
  * @brief  Enables or disables the AHB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it. 
  * @param  RCC_AHBPeriph: specifies the AHB2 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB2Periph_DCMI:   DCMI clock
  *            @arg RCC_AHB2Periph_CRYP:   CRYP clock
  *            @arg RCC_AHB2Periph_HASH:   HASH clock
  *            @arg RCC_AHB2Periph_RNG:    RNG clock
  *            @arg RCC_AHB2Periph_OTG_FS: USB OTG FS clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB2PeriphClockCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB2_PERIPH(RCC_AHB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->AHB2ENR |= RCC_AHB2Periph;
  }
  else
  {
    RCC->AHB2ENR &= ~RCC_AHB2Periph;
  }
}

/**
  * @brief  Enables or disables the AHB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it. 
  * @param  RCC_AHBPeriph: specifies the AHB3 peripheral to gates its clock.
  *          This parameter must be: RCC_AHB3Periph_FSMC
  *                                  or RCC_AHB3Periph_FMC (STM32F42xxx/43xxx devices)  
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB3PeriphClockCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB3_PERIPH(RCC_AHB3Periph));  
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->AHB3ENR |= RCC_AHB3Periph;
  }
  else
  {
    RCC->AHB3ENR &= ~RCC_AHB3Periph;
  }
}

/**
  * @brief  Enables or disables the Low Speed APB (APB1) peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it. 
  * @param  RCC_APB1Periph: specifies the APB1 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB1Periph_TIM2:   TIM2 clock
  *            @arg RCC_APB1Periph_TIM3:   TIM3 clock
  *            @arg RCC_APB1Periph_TIM4:   TIM4 clock
  *            @arg RCC_APB1Periph_TIM5:   TIM5 clock
  *            @arg RCC_APB1Periph_TIM6:   TIM6 clock
  *            @arg RCC_APB1Periph_TIM7:   TIM7 clock
  *            @arg RCC_APB1Periph_TIM12:  TIM12 clock
  *            @arg RCC_APB1Periph_TIM13:  TIM13 clock
  *            @arg RCC_APB1Periph_TIM14:  TIM14 clock
  *            @arg RCC_APB1Periph_WWDG:   WWDG clock
  *            @arg RCC_APB1Periph_SPI2:   SPI2 clock
  *            @arg RCC_APB1Periph_SPI3:   SPI3 clock
  *            @arg RCC_APB1Periph_USART2: USART2 clock
  *            @arg RCC_APB1Periph_USART3: USART3 clock
  *            @arg RCC_APB1Periph_UART4:  UART4 clock
  *            @arg RCC_APB1Periph_UART5:  UART5 clock
  *            @arg RCC_APB1Periph_I2C1:   I2C1 clock
  *            @arg RCC_APB1Periph_I2C2:   I2C2 clock
  *            @arg RCC_APB1Periph_I2C3:   I2C3 clock
  *            @arg RCC_APB1Periph_CAN1:   CAN1 clock
  *            @arg RCC_APB1Periph_CAN2:   CAN2 clock
  *            @arg RCC_APB1Periph_PWR:    PWR clock
  *            @arg RCC_APB1Periph_DAC:    DAC clock
  *            @arg RCC_APB1Periph_UART7:  UART7 clock
  *            @arg RCC_APB1Periph_UART8:  UART8 clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));  
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->APB1ENR |= RCC_APB1Periph;
  }
  else
  {
    RCC->APB1ENR &= ~RCC_APB1Periph;
  }
}

/**
  * @brief  Enables or disables the High Speed APB (APB2) peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.
  * @param  RCC_APB2Periph: specifies the APB2 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB2Periph_TIM1:   TIM1 clock
  *            @arg RCC_APB2Periph_TIM8:   TIM8 clock
  *            @arg RCC_APB2Periph_USART1: USART1 clock
  *            @arg RCC_APB2Periph_USART6: USART6 clock
  *            @arg RCC_APB2Periph_ADC1:   ADC1 clock
  *            @arg RCC_APB2Periph_ADC2:   ADC2 clock
  *            @arg RCC_APB2Periph_ADC3:   ADC3 clock
  *            @arg RCC_APB2Periph_SDIO:   SDIO clock
  *            @arg RCC_APB2Periph_SPI1:   SPI1 clock
  *            @arg RCC_APB2Periph_SPI4:   SPI4 clock
  *            @arg RCC_APB2Periph_SYSCFG: SYSCFG clock
  *            @arg RCC_APB2Periph_TIM9:   TIM9 clock
  *            @arg RCC_APB2Periph_TIM10:  TIM10 clock
  *            @arg RCC_APB2Periph_TIM11:  TIM11 clock
  *            @arg RCC_APB2Periph_SPI5:   SPI5 clock
  *            @arg RCC_APB2Periph_SPI6:   SPI6 clock
  *            @arg RCC_APB2Periph_SAI1:   SAI1 clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_APB2Periph_LTDC:   LTDC clock (STM32F429xx/439xx devices) 
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->APB2ENR |= RCC_APB2Periph;
  }
  else
  {
    RCC->APB2ENR &= ~RCC_APB2Periph;
  }
}

/**
  * @brief  Forces or releases AHB1 peripheral reset.
  * @param  RCC_AHB1Periph: specifies the AHB1 peripheral to reset.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB1Periph_GPIOA:   GPIOA clock
  *            @arg RCC_AHB1Periph_GPIOB:   GPIOB clock 
  *            @arg RCC_AHB1Periph_GPIOC:   GPIOC clock
  *            @arg RCC_AHB1Periph_GPIOD:   GPIOD clock
  *            @arg RCC_AHB1Periph_GPIOE:   GPIOE clock
  *            @arg RCC_AHB1Periph_GPIOF:   GPIOF clock
  *            @arg RCC_AHB1Periph_GPIOG:   GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOG:   GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOI:   GPIOI clock
  *            @arg RCC_AHB1Periph_GPIOJ:   GPIOJ clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_AHB1Periph_GPIOK:   GPIOK clock (STM32F42xxx/43xxxdevices)   
  *            @arg RCC_AHB1Periph_CRC:     CRC clock
  *            @arg RCC_AHB1Periph_DMA1:    DMA1 clock
  *            @arg RCC_AHB1Periph_DMA2:    DMA2 clock
  *            @arg RCC_AHB1Periph_DMA2D:   DMA2D clock (STM32F429xx/439xx devices)   
  *            @arg RCC_AHB1Periph_ETH_MAC: Ethernet MAC clock
  *            @arg RCC_AHB1Periph_OTG_HS:  USB OTG HS clock
  *                  
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB1PeriphResetCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB1_RESET_PERIPH(RCC_AHB1Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->AHB1RSTR |= RCC_AHB1Periph;
  }
  else
  {
    RCC->AHB1RSTR &= ~RCC_AHB1Periph;
  }
}

/**
  * @brief  Forces or releases AHB2 peripheral reset.
  * @param  RCC_AHB2Periph: specifies the AHB2 peripheral to reset.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB2Periph_DCMI:   DCMI clock
  *            @arg RCC_AHB2Periph_CRYP:   CRYP clock
  *            @arg RCC_AHB2Periph_HASH:   HASH clock
  *            @arg RCC_AHB2Periph_RNG:    RNG clock
  *            @arg RCC_AHB2Periph_OTG_FS: USB OTG FS clock
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB2PeriphResetCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB2_PERIPH(RCC_AHB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->AHB2RSTR |= RCC_AHB2Periph;
  }
  else
  {
    RCC->AHB2RSTR &= ~RCC_AHB2Periph;
  }
}

/**
  * @brief  Forces or releases AHB3 peripheral reset.
  * @param  RCC_AHB3Periph: specifies the AHB3 peripheral to reset.
  *          This parameter must be: RCC_AHB3Periph_FSMC
  *                                  or RCC_AHB3Periph_FMC (STM32F42xxx/43xxx devices)  
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB3PeriphResetCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB3_PERIPH(RCC_AHB3Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->AHB3RSTR |= RCC_AHB3Periph;
  }
  else
  {
    RCC->AHB3RSTR &= ~RCC_AHB3Periph;
  }
}

/**
  * @brief  Forces or releases Low Speed APB (APB1) peripheral reset.
  * @param  RCC_APB1Periph: specifies the APB1 peripheral to reset.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB1Periph_TIM2:   TIM2 clock
  *            @arg RCC_APB1Periph_TIM3:   TIM3 clock
  *            @arg RCC_APB1Periph_TIM4:   TIM4 clock
  *            @arg RCC_APB1Periph_TIM5:   TIM5 clock
  *            @arg RCC_APB1Periph_TIM6:   TIM6 clock
  *            @arg RCC_APB1Periph_TIM7:   TIM7 clock
  *            @arg RCC_APB1Periph_TIM12:  TIM12 clock
  *            @arg RCC_APB1Periph_TIM13:  TIM13 clock
  *            @arg RCC_APB1Periph_TIM14:  TIM14 clock
  *            @arg RCC_APB1Periph_WWDG:   WWDG clock
  *            @arg RCC_APB1Periph_SPI2:   SPI2 clock
  *            @arg RCC_APB1Periph_SPI3:   SPI3 clock
  *            @arg RCC_APB1Periph_USART2: USART2 clock
  *            @arg RCC_APB1Periph_USART3: USART3 clock
  *            @arg RCC_APB1Periph_UART4:  UART4 clock
  *            @arg RCC_APB1Periph_UART5:  UART5 clock
  *            @arg RCC_APB1Periph_I2C1:   I2C1 clock
  *            @arg RCC_APB1Periph_I2C2:   I2C2 clock
  *            @arg RCC_APB1Periph_I2C3:   I2C3 clock
  *            @arg RCC_APB1Periph_CAN1:   CAN1 clock
  *            @arg RCC_APB1Periph_CAN2:   CAN2 clock
  *            @arg RCC_APB1Periph_PWR:    PWR clock
  *            @arg RCC_APB1Periph_DAC:    DAC clock
  *            @arg RCC_APB1Periph_UART7:  UART7 clock
  *            @arg RCC_APB1Periph_UART8:  UART8 clock  
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->APB1RSTR |= RCC_APB1Periph;
  }
  else
  {
    RCC->APB1RSTR &= ~RCC_APB1Periph;
  }
}

/**
  * @brief  Forces or releases High Speed APB (APB2) peripheral reset.
  * @param  RCC_APB2Periph: specifies the APB2 peripheral to reset.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB2Periph_TIM1:   TIM1 clock
  *            @arg RCC_APB2Periph_TIM8:   TIM8 clock
  *            @arg RCC_APB2Periph_USART1: USART1 clock
  *            @arg RCC_APB2Periph_USART6: USART6 clock
  *            @arg RCC_APB2Periph_ADC1:   ADC1 clock
  *            @arg RCC_APB2Periph_ADC2:   ADC2 clock
  *            @arg RCC_APB2Periph_ADC3:   ADC3 clock
  *            @arg RCC_APB2Periph_SDIO:   SDIO clock
  *            @arg RCC_APB2Periph_SPI1:   SPI1 clock
  *            @arg RCC_APB2Periph_SPI4:   SPI4 clock  
  *            @arg RCC_APB2Periph_SYSCFG: SYSCFG clock
  *            @arg RCC_APB2Periph_TIM9:   TIM9 clock
  *            @arg RCC_APB2Periph_TIM10:  TIM10 clock
  *            @arg RCC_APB2Periph_TIM11:  TIM11 clock
  *            @arg RCC_APB2Periph_SPI5:   SPI5 clock
  *            @arg RCC_APB2Periph_SPI6:   SPI6 clock
  *            @arg RCC_APB2Periph_SAI1:   SAI1 clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_APB2Periph_LTDC:   LTDC clock (STM32F429xx/439xx devices)   
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB2_RESET_PERIPH(RCC_APB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->APB2RSTR |= RCC_APB2Periph;
  }
  else
  {
    RCC->APB2RSTR &= ~RCC_APB2Periph;
  }
}

/**
  * @brief  Enables or disables the AHB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @param  RCC_AHBPeriph: specifies the AHB1 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB1Periph_GPIOA:       GPIOA clock
  *            @arg RCC_AHB1Periph_GPIOB:       GPIOB clock 
  *            @arg RCC_AHB1Periph_GPIOC:       GPIOC clock
  *            @arg RCC_AHB1Periph_GPIOD:       GPIOD clock
  *            @arg RCC_AHB1Periph_GPIOE:       GPIOE clock
  *            @arg RCC_AHB1Periph_GPIOF:       GPIOF clock
  *            @arg RCC_AHB1Periph_GPIOG:       GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOG:       GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOI:       GPIOI clock
  *            @arg RCC_AHB1Periph_GPIOJ:       GPIOJ clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_AHB1Periph_GPIOK:       GPIOK clock (STM32F42xxx/43xxx devices)   
  *            @arg RCC_AHB1Periph_CRC:         CRC clock
  *            @arg RCC_AHB1Periph_BKPSRAM:     BKPSRAM interface clock
  *            @arg RCC_AHB1Periph_DMA1:        DMA1 clock
  *            @arg RCC_AHB1Periph_DMA2:        DMA2 clock
  *            @arg RCC_AHB1Periph_DMA2D:       DMA2D clock (STM32F429xx/439xx devices) 
  *            @arg RCC_AHB1Periph_ETH_MAC:     Ethernet MAC clock
  *            @arg RCC_AHB1Periph_ETH_MAC_Tx:  Ethernet Transmission clock
  *            @arg RCC_AHB1Periph_ETH_MAC_Rx:  Ethernet Reception clock
  *            @arg RCC_AHB1Periph_ETH_MAC_PTP: Ethernet PTP clock
  *            @arg RCC_AHB1Periph_OTG_HS:      USB OTG HS clock
  *            @arg RCC_AHB1Periph_OTG_HS_ULPI: USB OTG HS ULPI clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB1PeriphClockLPModeCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB1_LPMODE_PERIPH(RCC_AHB1Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB1LPENR |= RCC_AHB1Periph;
  }
  else
  {
    RCC->AHB1LPENR &= ~RCC_AHB1Periph;
  }
}

/**
  * @brief  Enables or disables the AHB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *           power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @param  RCC_AHBPeriph: specifies the AHB2 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB2Periph_DCMI:   DCMI clock
  *            @arg RCC_AHB2Periph_CRYP:   CRYP clock
  *            @arg RCC_AHB2Periph_HASH:   HASH clock
  *            @arg RCC_AHB2Periph_RNG:    RNG clock
  *            @arg RCC_AHB2Periph_OTG_FS: USB OTG FS clock  
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB2PeriphClockLPModeCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB2_PERIPH(RCC_AHB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB2LPENR |= RCC_AHB2Periph;
  }
  else
  {
    RCC->AHB2LPENR &= ~RCC_AHB2Periph;
  }
}

/**
  * @brief  Enables or disables the AHB3 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @param  RCC_AHBPeriph: specifies the AHB3 peripheral to gates its clock.
  *          This parameter must be: RCC_AHB3Periph_FSMC
  *                                  or RCC_AHB3Periph_FMC (STM32F429x/439x devices) 
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB3PeriphClockLPModeCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB3_PERIPH(RCC_AHB3Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB3LPENR |= RCC_AHB3Periph;
  }
  else
  {
    RCC->AHB3LPENR &= ~RCC_AHB3Periph;
  }
}

/**
  * @brief  Enables or disables the APB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @param  RCC_APB1Periph: specifies the APB1 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB1Periph_TIM2:   TIM2 clock
  *            @arg RCC_APB1Periph_TIM3:   TIM3 clock
  *            @arg RCC_APB1Periph_TIM4:   TIM4 clock
  *            @arg RCC_APB1Periph_TIM5:   TIM5 clock
  *            @arg RCC_APB1Periph_TIM6:   TIM6 clock
  *            @arg RCC_APB1Periph_TIM7:   TIM7 clock
  *            @arg RCC_APB1Periph_TIM12:  TIM12 clock
  *            @arg RCC_APB1Periph_TIM13:  TIM13 clock
  *            @arg RCC_APB1Periph_TIM14:  TIM14 clock
  *            @arg RCC_APB1Periph_WWDG:   WWDG clock
  *            @arg RCC_APB1Periph_SPI2:   SPI2 clock
  *            @arg RCC_APB1Periph_SPI3:   SPI3 clock
  *            @arg RCC_APB1Periph_USART2: USART2 clock
  *            @arg RCC_APB1Periph_USART3: USART3 clock
  *            @arg RCC_APB1Periph_UART4:  UART4 clock
  *            @arg RCC_APB1Periph_UART5:  UART5 clock
  *            @arg RCC_APB1Periph_I2C1:   I2C1 clock
  *            @arg RCC_APB1Periph_I2C2:   I2C2 clock
  *            @arg RCC_APB1Periph_I2C3:   I2C3 clock
  *            @arg RCC_APB1Periph_CAN1:   CAN1 clock
  *            @arg RCC_APB1Periph_CAN2:   CAN2 clock
  *            @arg RCC_APB1Periph_PWR:    PWR clock
  *            @arg RCC_APB1Periph_DAC:    DAC clock
  *            @arg RCC_APB1Periph_UART7:  UART7 clock
  *            @arg RCC_APB1Periph_UART8:  UART8 clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB1PeriphClockLPModeCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->APB1LPENR |= RCC_APB1Periph;
  }
  else
  {
    RCC->APB1LPENR &= ~RCC_APB1Periph;
  }
}

/**
  * @brief  Enables or disables the APB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @param  RCC_APB2Periph: specifies the APB2 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB2Periph_TIM1:   TIM1 clock
  *            @arg RCC_APB2Periph_TIM8:   TIM8 clock
  *            @arg RCC_APB2Periph_USART1: USART1 clock
  *            @arg RCC_APB2Periph_USART6: USART6 clock
  *            @arg RCC_APB2Periph_ADC1:   ADC1 clock
  *            @arg RCC_APB2Periph_ADC2:   ADC2 clock
  *            @arg RCC_APB2Periph_ADC3:   ADC3 clock
  *            @arg RCC_APB2Periph_SDIO:   SDIO clock
  *            @arg RCC_APB2Periph_SPI1:   SPI1 clock
  *            @arg RCC_APB2Periph_SPI4:   SPI4 clock
  *            @arg RCC_APB2Periph_SYSCFG: SYSCFG clock
  *            @arg RCC_APB2Periph_TIM9:   TIM9 clock
  *            @arg RCC_APB2Periph_TIM10:  TIM10 clock
  *            @arg RCC_APB2Periph_TIM11:  TIM11 clock
  *            @arg RCC_APB2Periph_SPI5:   SPI5 clock
  *            @arg RCC_APB2Periph_SPI6:   SPI6 clock
  *            @arg RCC_APB2Periph_SAI1:   SAI1 clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_APB2Periph_LTDC:   LTDC clock (STM32F429xx/439xx devices)   
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB2PeriphClockLPModeCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->APB2LPENR |= RCC_APB2Periph;
  }
  else
  {
    RCC->APB2LPENR &= ~RCC_APB2Periph;
  }
}

/**
  * @}
  */

/** @defgroup RCC_Group4 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions 
 *
@verbatim   
 ===============================================================================
                ##### Interrupts and flags management functions #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified RCC interrupts.
  * @param  RCC_IT: specifies the RCC interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_IT_LSIRDY: LSI ready interrupt
  *            @arg RCC_IT_LSERDY: LSE ready interrupt
  *            @arg RCC_IT_HSIRDY: HSI ready interrupt
  *            @arg RCC_IT_HSERDY: HSE ready interrupt
  *            @arg RCC_IT_PLLRDY: main PLL ready interrupt
  *            @arg RCC_IT_PLLI2SRDY: PLLI2S ready interrupt
  *            @arg RCC_IT_PLLSAIRDY: PLLSAI ready interrupt (only for STM32F42xxx/43xxx devices)
  * @param  NewState: new state of the specified RCC interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_IT(RCC_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Perform Byte access to RCC_CIR[14:8] bits to enable the selected interrupts */
    *(__IO uint8_t *) CIR_BYTE2_ADDRESS |= RCC_IT;
  }
  else
  {
    /* Perform Byte access to RCC_CIR[14:8] bits to disable the selected interrupts */
    *(__IO uint8_t *) CIR_BYTE2_ADDRESS &= (uint8_t)~RCC_IT;
  }
}

/**
  * @brief  Checks whether the specified RCC flag is set or not.
  * @param  RCC_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready
  *            @arg RCC_FLAG_HSERDY: HSE oscillator clock ready
  *            @arg RCC_FLAG_PLLRDY: main PLL clock ready
  *            @arg RCC_FLAG_PLLI2SRDY: PLLI2S clock ready
  *            @arg RCC_FLAG_PLLSAIRDY: PLLSAI clock ready (only for STM32F42xxx/43xxx devices)
  *            @arg RCC_FLAG_LSERDY: LSE oscillator clock ready
  *            @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready
  *            @arg RCC_FLAG_BORRST: POR/PDR or BOR reset
  *            @arg RCC_FLAG_PINRST: Pin reset
  *            @arg RCC_FLAG_PORRST: POR/PDR reset
  *            @arg RCC_FLAG_SFTRST: Software reset
  *            @arg RCC_FLAG_IWDGRST: Independent Watchdog reset
  *            @arg RCC_FLAG_WWDGRST: Window Watchdog reset
  *            @arg RCC_FLAG_LPWRRST: Low Power reset
  * @retval The new state of RCC_FLAG (SET or RESET).
  */
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG)
{
  uint32_t tmp = 0;
  uint32_t statusreg = 0;
  FlagStatus bitstatus = RESET;

  /* Check the parameters */
  assert_param(IS_RCC_FLAG(RCC_FLAG));

  /* Get the RCC register index */
  tmp = RCC_FLAG >> 5;
  if (tmp == 1)               /* The flag to check is in CR register */
  {
    statusreg = RCC->CR;
  }
  else if (tmp == 2)          /* The flag to check is in BDCR register */
  {
    statusreg = RCC->BDCR;
  }
  else                       /* The flag to check is in CSR register */
  {
    statusreg = RCC->CSR;
  }

  /* Get the flag position */
  tmp = RCC_FLAG & FLAG_MASK;
  if ((statusreg & ((uint32_t)1 << tmp)) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the flag status */
  return bitstatus;
}

/**
  * @brief  Clears the RCC reset flags.
  *         The reset flags are: RCC_FLAG_PINRST, RCC_FLAG_PORRST,  RCC_FLAG_SFTRST,
  *         RCC_FLAG_IWDGRST, RCC_FLAG_WWDGRST, RCC_FLAG_LPWRRST
  * @param  None
  * @retval None
  */
void RCC_ClearFlag(void)
{
  /* Set RMVF bit to clear the reset flags */
  RCC->CSR |= RCC_CSR_RMVF;
}

/**
  * @brief  Checks whether the specified RCC interrupt has occurred or not.
  * @param  RCC_IT: specifies the RCC interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg RCC_IT_LSIRDY: LSI ready interrupt
  *            @arg RCC_IT_LSERDY: LSE ready interrupt
  *            @arg RCC_IT_HSIRDY: HSI ready interrupt
  *            @arg RCC_IT_HSERDY: HSE ready interrupt
  *            @arg RCC_IT_PLLRDY: main PLL ready interrupt
  *            @arg RCC_IT_PLLI2SRDY: PLLI2S ready interrupt           
  *            @arg RCC_IT_PLLSAIRDY: PLLSAI clock ready interrupt (only for STM32F42xxx/43xxx devices)    
  *            @arg RCC_IT_CSS: Clock Security System interrupt
  * @retval The new state of RCC_IT (SET or RESET).
  */
ITStatus RCC_GetITStatus(uint8_t RCC_IT)
{
  ITStatus bitstatus = RESET;

  /* Check the parameters */
  assert_param(IS_RCC_GET_IT(RCC_IT));

  /* Check the status of the specified RCC interrupt */
  if ((RCC->CIR & RCC_IT) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the RCC_IT status */
  return  bitstatus;
}

/**
  * @brief  Clears the RCC's interrupt pending bits.
  * @param  RCC_IT: specifies the interrupt pending bit to clear.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_IT_LSIRDY: LSI ready interrupt
  *            @arg RCC_IT_LSERDY: LSE ready interrupt
  *            @arg RCC_IT_HSIRDY: HSI ready interrupt
  *            @arg RCC_IT_HSERDY: HSE ready interrupt
  *            @arg RCC_IT_PLLRDY: main PLL ready interrupt
  *            @arg RCC_IT_PLLI2SRDY: PLLI2S ready interrupt  
  *            @arg RCC_IT_PLLSAIRDY: PLLSAI ready interrupt (only for STM32F42xxx/43xxx devices)   
  *            @arg RCC_IT_CSS: Clock Security System interrupt
  * @retval None
  */
void RCC_ClearITPendingBit(uint8_t RCC_IT)
{
  /* Check the parameters */
  assert_param(IS_RCC_CLEAR_IT(RCC_IT));

  /* Perform Byte access to RCC_CIR[23:16] bits to clear the selected interrupt
     pending bits */
  *(__IO uint8_t *) CIR_BYTE3_ADDRESS = RCC_IT;
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

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
