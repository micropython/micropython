/**
  ******************************************************************************
  * @file    stm32f4xx_hal_pwr_ex.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   Extended PWR HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of PWR extension peripheral:           
  *           + Peripheral Extended features functions
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

/** @defgroup PWREx 
  * @brief PWR HAL module driver
  * @{
  */

#ifdef HAL_PWR_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PWR_OVERDRIVE_TIMEOUT_VALUE  1000
#define PWR_BKPREG_TIMEOUT_VALUE     1000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup PWREx_Private_Functions
  * @{
  */

/** @defgroup PWREx_Group1 Peripheral Extended features functions 
  *  @brief Peripheral Extended features functions 
  *
@verbatim   

 ===============================================================================
                 ##### Peripheral extended features functions #####
 ===============================================================================

    *** Main and Backup Regulators configuration ***
    ================================================
    [..] 
      (+) The backup domain includes 4 Kbytes of backup SRAM accessible only from 
          the CPU, and address in 32-bit, 16-bit or 8-bit mode. Its content is 
          retained even in Standby or VBAT mode when the low power backup regulator
          is enabled. It can be considered as an internal EEPROM when VBAT is 
          always present. You can use the HAL_PWR_EnableBkUpReg() function to 
          enable the low power backup regulator. 

      (+) When the backup domain is supplied by VDD (analog switch connected to VDD) 
          the backup SRAM is powered from VDD which replaces the VBAT power supply to 
          save battery life.

      (+) The backup SRAM is not mass erased by a tamper event. It is read 
          protected to prevent confidential data, such as cryptographic private 
          key, from being accessed. The backup SRAM can be erased only through 
          the Flash interface when a protection level change from level 1 to 
          level 0 is requested. 
      -@- Refer to the description of Read protection (RDP) in the Flash 
          programming manual.

      (+) The main internal regulator can be configured to have a tradeoff between 
          performance and power consumption when the device does not operate at 
          the maximum frequency. This is done through __HAL_PWR_MAINREGULATORMODE_CONFIG() 
          macro which configure VOS bit in PWR_CR register
          
        Refer to the product datasheets for more details.

    *** FLASH Power Down configuration ****
    =======================================
    [..] 
      (+) By setting the FPDS bit in the PWR_CR register by using the 
          HAL_PWR_EnableFlashPowerDown() function, the Flash memory also enters power 
          down mode when the device enters Stop mode. When the Flash memory 
          is in power down mode, an additional startup delay is incurred when 
          waking up from Stop mode.
          
           (+) For STM32F42xxx/43xxx Devices, the scale can be modified only when the PLL 
           is OFF and the HSI or HSE clock source is selected as system clock. 
           The new value programmed is active only when the PLL is ON.
           When the PLL is OFF, the voltage scale 3 is automatically selected. 
        Refer to the datasheets for more details.

    *** Over-Drive and Under-Drive configuration ****
    =================================================
    [..]         
       (+) For STM32F42xxx/43xxx Devices, in Run mode: the main regulator has
           2 operating modes available:
        (++) Normal mode: The CPU and core logic operate at maximum frequency at a given 
             voltage scaling (scale 1, scale 2 or scale 3)
        (++) Over-drive mode: This mode allows the CPU and the core logic to operate at a 
            higher frequency than the normal mode for a given voltage scaling (scale 1,  
            scale 2 or scale 3). This mode is enabled through HAL_PWREx_EnableOverDrive() function and
            disabled by HAL_PWREx_DisableOverDrive() function, to enter or exit from Over-drive mode please follow 
            the sequence described in Reference manual.
             
       (+) For STM32F42xxx/43xxx Devices, in Stop mode: the main regulator or low power regulator 
           supplies a low power voltage to the 1.2V domain, thus preserving the content of registers 
           and internal SRAM. 2 operating modes are available:
         (++) Normal mode: the 1.2V domain is preserved in nominal leakage mode. This mode is only 
              available when the main regulator or the low power regulator is used in Scale 3 or 
              low voltage mode.
         (++) Under-drive mode: the 1.2V domain is preserved in reduced leakage mode. This mode is only
              available when the main regulator or the low power regulator is in low voltage mode.

@endverbatim
  * @{
  */

/**
  * @brief Enables the Backup Regulator.
  * @param None
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PWREx_EnableBkUpReg(void)
{
  uint32_t timeout = 0;   

  *(__IO uint32_t *) CSR_BRE_BB = (uint32_t)ENABLE;

  /* Get timeout */
  timeout = HAL_GetTick() + PWR_BKPREG_TIMEOUT_VALUE;
  /* Wait till Backup regulator ready flag is set */  
  while(__HAL_PWR_GET_FLAG(PWR_FLAG_BRR) == RESET)
  {
    if(HAL_GetTick() >= timeout)
    {
      return HAL_TIMEOUT;
    } 
  }
  return HAL_OK;
}

/**
  * @brief Disables the Backup Regulator.
  * @param None
  * @retval None
  */
HAL_StatusTypeDef HAL_PWREx_DisableBkUpReg(void)
{
  uint32_t timeout = 0; 

  *(__IO uint32_t *) CSR_BRE_BB = (uint32_t)DISABLE;

  /* Get timeout */
  timeout = HAL_GetTick() + PWR_BKPREG_TIMEOUT_VALUE;
  /* Wait till Backup regulator ready flag is set */  
  while(__HAL_PWR_GET_FLAG(PWR_FLAG_BRR) != RESET)
  {
    if(HAL_GetTick() >= timeout)
    {
      return HAL_TIMEOUT;
    } 
  }
  return HAL_OK;
}

/**
  * @brief Enables the Flash Power Down in Stop mode.
  * @param None
  * @retval None
  */
void HAL_PWREx_EnableFlashPowerDown(void)
{
  *(__IO uint32_t *) CR_FPDS_BB = (uint32_t)ENABLE;
}

/**
  * @brief Disables the Flash Power Down in Stop mode.
  * @param None
  * @retval None
  */
void HAL_PWREx_DisableFlashPowerDown(void)
{
  *(__IO uint32_t *) CR_FPDS_BB = (uint32_t)DISABLE;
}

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
/**
  * @brief  Activates the Over-Drive mode.
  * @note   These macros can be used only for STM32F42xx/STM32F43xx devices.
  *         This mode allows the CPU and the core logic to operate at a higher frequency
  *         than the normal mode for a given voltage scaling (scale 1, scale 2 or scale 3).   
  * @note   It is recommended to enter or exit Over-drive mode when the application is not running 
  *         critical tasks and when the system clock source is either HSI or HSE. 
  *         During the Over-drive switch activation, no peripheral clocks should be enabled.   
  *         The peripheral clocks must be enabled once the Over-drive mode is activated.   
  * @param  None
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PWREx_ActivateOverDrive(void)
{
  uint32_t timeout = 0;
  
  __PWR_CLK_ENABLE();
  
  /* Enable the Over-drive to extend the clock frequency to 180 Mhz */
  __HAL_PWR_OVERDRIVE_ENABLE();
  
  /* Get timeout */
  timeout = HAL_GetTick() + PWR_OVERDRIVE_TIMEOUT_VALUE;
  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_ODRDY))
  {
    if(HAL_GetTick() >= timeout)
    {
      return HAL_TIMEOUT;
    }
  }
  
  /* Enable the Over-drive switch */
  __HAL_PWR_OVERDRIVESWITCHING_ENABLE();
  
  /* Get timeout */
  timeout = HAL_GetTick() + PWR_OVERDRIVE_TIMEOUT_VALUE;
  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_ODSWRDY))
  {
    if(HAL_GetTick() >= timeout)
    {
      return HAL_TIMEOUT;
    }
  } 
  return HAL_OK;
}

/**
  * @brief  Deactivates the Over-Drive mode.
  * @note   These macros can be used only for STM32F42xx/STM32F43xx devices.
  *         This mode allows the CPU and the core logic to operate at a higher frequency
  *         than the normal mode for a given voltage scaling (scale 1, scale 2 or scale 3).    
  * @note   It is recommended to enter or exit Over-drive mode when the application is not running 
  *         critical tasks and when the system clock source is either HSI or HSE. 
  *         During the Over-drive switch activation, no peripheral clocks should be enabled.   
  *         The peripheral clocks must be enabled once the Over-drive mode is activated.
  * @param  None
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PWREx_DeactivateOverDrive(void)
{
  uint32_t timeout = 0;
  
  __PWR_CLK_ENABLE();
    
  /* Disable the Over-drive switch */
  __HAL_PWR_OVERDRIVESWITCHING_DISABLE();
  
  /* Get timeout */
  timeout = HAL_GetTick() + PWR_OVERDRIVE_TIMEOUT_VALUE;
 
  while(__HAL_PWR_GET_FLAG(PWR_FLAG_ODSWRDY))
  {
    if(HAL_GetTick() >= timeout)
    {
      return HAL_TIMEOUT;
    }
  } 
  
  /* Disable the Over-drive */
  __HAL_PWR_OVERDRIVE_DISABLE();

  /* Get timeout */
  timeout = HAL_GetTick() + PWR_OVERDRIVE_TIMEOUT_VALUE;  

  while(__HAL_PWR_GET_FLAG(PWR_FLAG_ODRDY))
  {
    if(HAL_GetTick() >= timeout)
    {
      return HAL_TIMEOUT;
    }
  }
  
  return HAL_OK;
}
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_PWR_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
