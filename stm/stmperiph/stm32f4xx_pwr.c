/**
  ******************************************************************************
  * @file    stm32f4xx_pwr.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    08-November-2013
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Power Controller (PWR) peripheral:           
  *           + Backup Domain Access
  *           + PVD configuration
  *           + WakeUp pin configuration
  *           + Main and Backup Regulators configuration
  *           + FLASH Power Down configuration
  *           + Low Power modes configuration
  *           + Flags management
  *               
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
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup PWR 
  * @brief PWR driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* --------- PWR registers bit address in the alias region ---------- */
#define PWR_OFFSET               (PWR_BASE - PERIPH_BASE)

/* --- CR Register ---*/

/* Alias word address of DBP bit */
#define CR_OFFSET                (PWR_OFFSET + 0x00)
#define DBP_BitNumber            0x08
#define CR_DBP_BB                (PERIPH_BB_BASE + (CR_OFFSET * 32) + (DBP_BitNumber * 4))

/* Alias word address of PVDE bit */
#define PVDE_BitNumber           0x04
#define CR_PVDE_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PVDE_BitNumber * 4))

/* Alias word address of FPDS bit */
#define FPDS_BitNumber           0x09
#define CR_FPDS_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (FPDS_BitNumber * 4))

/* Alias word address of PMODE bit */
#define PMODE_BitNumber           0x0E
#define CR_PMODE_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PMODE_BitNumber * 4))

/* Alias word address of ODEN bit */
#define ODEN_BitNumber           0x10
#define CR_ODEN_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (ODEN_BitNumber * 4))

/* Alias word address of ODSWEN bit */
#define ODSWEN_BitNumber         0x11
#define CR_ODSWEN_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (ODSWEN_BitNumber * 4))

/* --- CSR Register ---*/

/* Alias word address of EWUP bit */
#define CSR_OFFSET               (PWR_OFFSET + 0x04)
#define EWUP_BitNumber           0x08
#define CSR_EWUP_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (EWUP_BitNumber * 4))

/* Alias word address of BRE bit */
#define BRE_BitNumber            0x09
#define CSR_BRE_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (BRE_BitNumber * 4))

/* ------------------ PWR registers bit mask ------------------------ */

/* CR register bit mask */
#define CR_DS_MASK               ((uint32_t)0xFFFFF3FC)
#define CR_PLS_MASK              ((uint32_t)0xFFFFFF1F)
#define CR_VOS_MASK              ((uint32_t)0xFFFF3FFF)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup PWR_Private_Functions
  * @{
  */

/** @defgroup PWR_Group1 Backup Domain Access function 
 *  @brief   Backup Domain Access function  
 *
@verbatim   
 ===============================================================================
                  ##### Backup Domain Access function #####
 ===============================================================================  
    [..]
      After reset, the backup domain (RTC registers, RTC backup data 
      registers and backup SRAM) is protected against possible unwanted 
      write accesses. 
      To enable access to the RTC Domain and RTC registers, proceed as follows:
        (+) Enable the Power Controller (PWR) APB1 interface clock using the
            RCC_APB1PeriphClockCmd() function.
        (+) Enable access to RTC domain using the PWR_BackupAccessCmd() function.

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the PWR peripheral registers to their default reset values.     
  * @param  None
  * @retval None
  */
void PWR_DeInit(void)
{
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, DISABLE);
}

/**
  * @brief  Enables or disables access to the backup domain (RTC registers, RTC 
  *         backup data registers and backup SRAM).
  * @note   If the HSE divided by 2, 3, ..31 is used as the RTC clock, the 
  *         Backup Domain Access should be kept enabled.
  * @param  NewState: new state of the access to the backup domain.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_BackupAccessCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  *(__IO uint32_t *) CR_DBP_BB = (uint32_t)NewState;
}

/**
  * @}
  */

/** @defgroup PWR_Group2 PVD configuration functions
 *  @brief   PVD configuration functions 
 *
@verbatim   
 ===============================================================================
                    ##### PVD configuration functions #####
 ===============================================================================  
    [..]
      (+) The PVD is used to monitor the VDD power supply by comparing it to a 
          threshold selected by the PVD Level (PLS[2:0] bits in the PWR_CR).
      (+) A PVDO flag is available to indicate if VDD/VDDA is higher or lower 
          than the PVD threshold. This event is internally connected to the EXTI 
          line16 and can generate an interrupt if enabled through the EXTI registers.
      (+) The PVD is stopped in Standby mode.

@endverbatim
  * @{
  */

/**
  * @brief  Configures the voltage threshold detected by the Power Voltage Detector(PVD).
  * @param  PWR_PVDLevel: specifies the PVD detection level
  *          This parameter can be one of the following values:
  *            @arg PWR_PVDLevel_0
  *            @arg PWR_PVDLevel_1
  *            @arg PWR_PVDLevel_2
  *            @arg PWR_PVDLevel_3
  *            @arg PWR_PVDLevel_4
  *            @arg PWR_PVDLevel_5
  *            @arg PWR_PVDLevel_6
  *            @arg PWR_PVDLevel_7
  * @note   Refer to the electrical characteristics of your device datasheet for
  *         more details about the voltage threshold corresponding to each 
  *         detection level.
  * @retval None
  */
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_PWR_PVD_LEVEL(PWR_PVDLevel));
  
  tmpreg = PWR->CR;
  
  /* Clear PLS[7:5] bits */
  tmpreg &= CR_PLS_MASK;
  
  /* Set PLS[7:5] bits according to PWR_PVDLevel value */
  tmpreg |= PWR_PVDLevel;
  
  /* Store the new value */
  PWR->CR = tmpreg;
}

/**
  * @brief  Enables or disables the Power Voltage Detector(PVD).
  * @param  NewState: new state of the PVD.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_PVDCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  *(__IO uint32_t *) CR_PVDE_BB = (uint32_t)NewState;
}

/**
  * @}
  */

/** @defgroup PWR_Group3 WakeUp pin configuration functions
 *  @brief   WakeUp pin configuration functions 
 *
@verbatim   
 ===============================================================================
                 ##### WakeUp pin configuration functions #####
 ===============================================================================  
    [..]
      (+) WakeUp pin is used to wakeup the system from Standby mode. This pin is 
          forced in input pull down configuration and is active on rising edges.
      (+) There is only one WakeUp pin: WakeUp Pin 1 on PA.00.

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the WakeUp Pin functionality.
  * @param  NewState: new state of the WakeUp Pin functionality.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_WakeUpPinCmd(FunctionalState NewState)
{
  /* Check the parameters */  
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  *(__IO uint32_t *) CSR_EWUP_BB = (uint32_t)NewState;
}

/**
  * @}
  */

/** @defgroup PWR_Group4 Main and Backup Regulators configuration functions
 *  @brief   Main and Backup Regulators configuration functions 
 *
@verbatim   
 ===============================================================================
          ##### Main and Backup Regulators configuration functions #####
 ===============================================================================  
    [..]
      (+) The backup domain includes 4 Kbytes of backup SRAM accessible only from 
          the CPU, and address in 32-bit, 16-bit or 8-bit mode. Its content is 
          retained even in Standby or VBAT mode when the low power backup regulator
          is enabled. It can be considered as an internal EEPROM when VBAT is 
          always present. You can use the PWR_BackupRegulatorCmd() function to 
          enable the low power backup regulator and use the PWR_GetFlagStatus
          (PWR_FLAG_BRR) to check if it is ready or not. 

      (+) When the backup domain is supplied by VDD (analog switch connected to VDD) 
          the backup SRAM is powered from VDD which replaces the VBAT power supply to 
          save battery life.

      (+) The backup SRAM is not mass erased by an tamper event. It is read 
          protected to prevent confidential data, such as cryptographic private 
          key, from being accessed. The backup SRAM can be erased only through 
          the Flash interface when a protection level change from level 1 to 
          level 0 is requested. 
      -@- Refer to the description of Read protection (RDP) in the reference manual.

      (+) The main internal regulator can be configured to have a tradeoff between 
          performance and power consumption when the device does not operate at 
          the maximum frequency. 
      (+) For STM32F405xx/407xx and STM32F415xx/417xx  Devices, the regulator can be     
          configured on the fly through PWR_MainRegulatorModeConfig() function which  
          configure VOS bit in PWR_CR register:
        (++) When this bit is set (Regulator voltage output Scale 1 mode selected) 
             the System frequency can go up to 168 MHz. 
        (++) When this bit is reset (Regulator voltage output Scale 2 mode selected) 
             the System frequency can go up to 144 MHz.
             
       (+) For STM32F42xxx/43xxx Devices, the regulator can be configured through    
           PWR_MainRegulatorModeConfig() function which configure VOS[1:0] bits in
           PWR_CR register:  
           which configure VOS[1:0] bits in PWR_CR register: 
        (++) When VOS[1:0] = 11 (Regulator voltage output Scale 1 mode selected) 
             the System frequency can go up to 168 MHz. 
        (++) When VOS[1:0] = 10 (Regulator voltage output Scale 2 mode selected) 
             the System frequency can go up to 144 MHz.  
        (++) When VOS[1:0] = 01 (Regulator voltage output Scale 3 mode selected) 
             the System frequency can go up to 120 MHz. 
                          
       (+) For STM32F42xxx/43xxx Devices, the scale can be modified only when the PLL 
           is OFF and the HSI or HSE clock source is selected as system clock. 
           The new value programmed is active only when the PLL is ON.
           When the PLL is OFF, the voltage scale 3 is automatically selected. 
        Refer to the datasheets for more details.
        
       (+) For STM32F42xxx/43xxx Devices, in Run mode: the main regulator has
           2 operating modes available:
        (++) Normal mode: The CPU and core logic operate at maximum frequency at a given 
             voltage scaling (scale 1, scale 2 or scale 3)
        (++) Over-drive mode: This mode allows the CPU and the core logic to operate at a 
            higher frequency than the normal mode for a given voltage scaling (scale 1,  
            scale 2 or scale 3). This mode is enabled through PWR_OverDriveCmd() function and
            PWR_OverDriveSWCmd() function, to enter or exit from Over-drive mode please follow 
            the sequence described in Reference manual.
             
       (+) For STM32F42xxx/43xxx Devices, in Stop mode: the main regulator or low power regulator 
           supplies a low power voltage to the 1.2V domain, thus preserving the content of registers 
           and internal SRAM. 2 operating modes are available:
         (++) Normal mode: the 1.2V domain is preserved in nominal leakage mode. This mode is only 
              available when the main regulator or the low power regulator is used in Scale 3 or 
              low voltage mode.
         (++) Under-drive mode: the 1.2V domain is preserved in reduced leakage mode. This mode is only
              available when the main regulator or the low power regulator is in low voltage mode.
              This mode is enabled through PWR_UnderDriveCmd() function.
            
@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the Backup Regulator.
  * @param  NewState: new state of the Backup Regulator.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_BackupRegulatorCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  *(__IO uint32_t *) CSR_BRE_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the main internal regulator output voltage.
  * @param  PWR_Regulator_Voltage: specifies the regulator output voltage to achieve
  *         a tradeoff between performance and power consumption when the device does
  *         not operate at the maximum frequency (refer to the datasheets for more details).
  *          This parameter can be one of the following values:
  *            @arg PWR_Regulator_Voltage_Scale1: Regulator voltage output Scale 1 mode, 
  *                                                System frequency up to 168 MHz. 
  *            @arg PWR_Regulator_Voltage_Scale2: Regulator voltage output Scale 2 mode, 
  *                                                System frequency up to 144 MHz.    
  *            @arg PWR_Regulator_Voltage_Scale3: Regulator voltage output Scale 3 mode, 
  *                                                System frequency up to 120 MHz (only for STM32F42xxx/43xxx devices)
  * @retval None
  */
void PWR_MainRegulatorModeConfig(uint32_t PWR_Regulator_Voltage)
{
  uint32_t tmpreg = 0;
	
  /* Check the parameters */
  assert_param(IS_PWR_REGULATOR_VOLTAGE(PWR_Regulator_Voltage));

  tmpreg = PWR->CR;
  
  /* Clear VOS[15:14] bits */
  tmpreg &= CR_VOS_MASK;
  
  /* Set VOS[15:14] bits according to PWR_Regulator_Voltage value */
  tmpreg |= PWR_Regulator_Voltage;
  
  /* Store the new value */
  PWR->CR = tmpreg;
}

/**
  * @brief  Enables or disables the Over-Drive.
  * 
  * @note   This function can be used only for STM32F42xxx/STM3243xxx devices.
  *         This mode allows the CPU and the core logic to operate at a higher frequency
  *         than the normal mode for a given voltage scaling (scale 1, scale 2 or scale 3).   
  * 
  * @note   It is recommended to enter or exit Over-drive mode when the application is not running 
  *          critical tasks and when the system clock source is either HSI or HSE. 
  *          During the Over-drive switch activation, no peripheral clocks should be enabled.   
  *          The peripheral clocks must be enabled once the Over-drive mode is activated.
  *            
  * @param  NewState: new state of the Over Drive mode.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_OverDriveCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* Set/Reset the ODEN bit to enable/disable the Over Drive mode */
  *(__IO uint32_t *) CR_ODEN_BB = (uint32_t)NewState;
}

/**
  * @brief  Enables or disables the Over-Drive switching.
  * 
  * @note   This function can be used only for STM32F42xxx/STM3243xxx devices. 
  *       
  * @param  NewState: new state of the Over Drive switching mode.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_OverDriveSWCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set/Reset the ODSWEN bit to enable/disable the Over Drive switching mode */
  *(__IO uint32_t *) CR_ODSWEN_BB = (uint32_t)NewState;
}

/**
  * @brief   Enables or disables the Under-Drive mode.
  * 
  * @note   This function can be used only for STM32F42xxx/STM3243xxx devices.
  * @note    This mode is enabled only with STOP low power mode.
  *          In this mode, the 1.2V domain is preserved in reduced leakage mode. This 
  *          mode is only available when the main regulator or the low power regulator 
  *          is in low voltage mode
  *        
  * @note   If the Under-drive mode was enabled, it is automatically disabled after 
  *         exiting Stop mode. 
  *         When the voltage regulator operates in Under-drive mode, an additional  
  *         startup delay is induced when waking up from Stop mode.
  *                    
  * @param  NewState: new state of the Under Drive mode.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_UnderDriveCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Set the UDEN[1:0] bits to enable the Under Drive mode */
    PWR->CR |= (uint32_t)PWR_CR_UDEN;
  }
  else
  {
    /* Reset the UDEN[1:0] bits to disable the Under Drive mode */
    PWR->CR &= (uint32_t)(~PWR_CR_UDEN);
  }
}

/**
  * @}
  */

/** @defgroup PWR_Group5 FLASH Power Down configuration functions
 *  @brief   FLASH Power Down configuration functions 
 *
@verbatim   
 ===============================================================================
             ##### FLASH Power Down configuration functions #####
 ===============================================================================  
    [..]
      (+) By setting the FPDS bit in the PWR_CR register by using the 
          PWR_FlashPowerDownCmd() function, the Flash memory also enters power 
          down mode when the device enters Stop mode. When the Flash memory 
          is in power down mode, an additional startup delay is incurred when 
          waking up from Stop mode.
@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the Flash Power Down in STOP mode.
  * @param  NewState: new state of the Flash power mode.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_FlashPowerDownCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  *(__IO uint32_t *) CR_FPDS_BB = (uint32_t)NewState;
}

/**
  * @}
  */

/** @defgroup PWR_Group6 Low Power modes configuration functions
 *  @brief   Low Power modes configuration functions 
 *
@verbatim   
 ===============================================================================
              ##### Low Power modes configuration functions #####
 ===============================================================================  
    [..]
      The devices feature 3 low-power modes:
      (+) Sleep mode: Cortex-M4 core stopped, peripherals kept running.
      (+) Stop mode: all clocks are stopped, regulator running, regulator 
          in low power mode
      (+) Standby mode: 1.2V domain powered off.
   
   *** Sleep mode ***
   ==================
    [..]
      (+) Entry:
        (++) The Sleep mode is entered by using the __WFI() or __WFE() functions.
      (+) Exit:
        (++) Any peripheral interrupt acknowledged by the nested vectored interrupt 
             controller (NVIC) can wake up the device from Sleep mode.

   *** Stop mode ***
   =================
    [..]
      In Stop mode, all clocks in the 1.2V domain are stopped, the PLL, the HSI,
      and the HSE RC oscillators are disabled. Internal SRAM and register contents 
      are preserved.
      The voltage regulator can be configured either in normal or low-power mode.
      To minimize the consumption In Stop mode, FLASH can be powered off before 
      entering the Stop mode. It can be switched on again by software after exiting 
      the Stop mode using the PWR_FlashPowerDownCmd() function. 
   
      (+) Entry:
        (++) The Stop mode is entered using the PWR_EnterSTOPMode(PWR_MainRegulator_ON) 
             function with:
          (+++) Main regulator ON.
          (+++) Low Power regulator ON.
      (+) Exit:
        (++) Any EXTI Line (Internal or External) configured in Interrupt/Event mode.
      
   *** Standby mode ***
   ====================
    [..]
      The Standby mode allows to achieve the lowest power consumption. It is based 
      on the Cortex-M4 deepsleep mode, with the voltage regulator disabled. 
      The 1.2V domain is consequently powered off. The PLL, the HSI oscillator and 
      the HSE oscillator are also switched off. SRAM and register contents are lost 
      except for the RTC registers, RTC backup registers, backup SRAM and Standby 
      circuitry.
   
      The voltage regulator is OFF.
      
      (+) Entry:
        (++) The Standby mode is entered using the PWR_EnterSTANDBYMode() function.
      (+) Exit:
        (++) WKUP pin rising edge, RTC alarm (Alarm A and Alarm B), RTC wakeup,
             tamper event, time-stamp event, external reset in NRST pin, IWDG reset.              

   *** Auto-wakeup (AWU) from low-power mode ***
   =============================================
    [..]
      The MCU can be woken up from low-power mode by an RTC Alarm event, an RTC 
      Wakeup event, a tamper event, a time-stamp event, or a comparator event, 
      without depending on an external interrupt (Auto-wakeup mode).

      (#) RTC auto-wakeup (AWU) from the Stop mode
       
        (++) To wake up from the Stop mode with an RTC alarm event, it is necessary to:
          (+++) Configure the EXTI Line 17 to be sensitive to rising edges (Interrupt 
                or Event modes) using the EXTI_Init() function.
          (+++) Enable the RTC Alarm Interrupt using the RTC_ITConfig() function
          (+++) Configure the RTC to generate the RTC alarm using the RTC_SetAlarm() 
                and RTC_AlarmCmd() functions.
        (++) To wake up from the Stop mode with an RTC Tamper or time stamp event, it 
             is necessary to:
          (+++) Configure the EXTI Line 21 to be sensitive to rising edges (Interrupt 
                or Event modes) using the EXTI_Init() function.
          (+++) Enable the RTC Tamper or time stamp Interrupt using the RTC_ITConfig() 
                function
          (+++) Configure the RTC to detect the tamper or time stamp event using the
                RTC_TimeStampConfig(), RTC_TamperTriggerConfig() and RTC_TamperCmd()
                functions.
        (++) To wake up from the Stop mode with an RTC WakeUp event, it is necessary to:
           (+++) Configure the EXTI Line 22 to be sensitive to rising edges (Interrupt 
                 or Event modes) using the EXTI_Init() function.
           (+++) Enable the RTC WakeUp Interrupt using the RTC_ITConfig() function
           (+++) Configure the RTC to generate the RTC WakeUp event using the RTC_WakeUpClockConfig(), 
                 RTC_SetWakeUpCounter() and RTC_WakeUpCmd() functions.

      (#) RTC auto-wakeup (AWU) from the Standby mode
   
        (++) To wake up from the Standby mode with an RTC alarm event, it is necessary to:
          (+++) Enable the RTC Alarm Interrupt using the RTC_ITConfig() function
          (+++) Configure the RTC to generate the RTC alarm using the RTC_SetAlarm() 
                and RTC_AlarmCmd() functions.
        (++) To wake up from the Standby mode with an RTC Tamper or time stamp event, it 
             is necessary to:
          (+++) Enable the RTC Tamper or time stamp Interrupt using the RTC_ITConfig() 
                function
          (+++) Configure the RTC to detect the tamper or time stamp event using the
                RTC_TimeStampConfig(), RTC_TamperTriggerConfig() and RTC_TamperCmd()
                functions.
        (++) To wake up from the Standby mode with an RTC WakeUp event, it is necessary to:
          (+++) Enable the RTC WakeUp Interrupt using the RTC_ITConfig() function
          (+++) Configure the RTC to generate the RTC WakeUp event using the RTC_WakeUpClockConfig(), 
                RTC_SetWakeUpCounter() and RTC_WakeUpCmd() functions.

@endverbatim
  * @{
  */

/**
  * @brief  Enters STOP mode.
  *   
  * @note   In Stop mode, all I/O pins keep the same state as in Run mode.
  * @note   When exiting Stop mode by issuing an interrupt or a wakeup event, 
  *         the HSI RC oscillator is selected as system clock.
  * @note   When the voltage regulator operates in low power mode, an additional 
  *         startup delay is incurred when waking up from Stop mode. 
  *         By keeping the internal regulator ON during Stop mode, the consumption 
  *         is higher although the startup time is reduced.
  *     
  * @param  PWR_Regulator: specifies the regulator state in STOP mode.
  *          This parameter can be one of the following values:
  *            @arg PWR_MainRegulator_ON: STOP mode with regulator ON
  *            @arg PWR_LowPowerRegulator_ON: STOP mode with low power regulator ON
  * @param  PWR_STOPEntry: specifies if STOP mode in entered with WFI or WFE instruction.
  *          This parameter can be one of the following values:
  *            @arg PWR_STOPEntry_WFI: enter STOP mode with WFI instruction
  *            @arg PWR_STOPEntry_WFE: enter STOP mode with WFE instruction
  * @retval None
  */
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_PWR_REGULATOR(PWR_Regulator));
  assert_param(IS_PWR_STOP_ENTRY(PWR_STOPEntry));
  
  /* Select the regulator state in STOP mode ---------------------------------*/
  tmpreg = PWR->CR;
  /* Clear PDDS and LPDS bits */
  tmpreg &= CR_DS_MASK;
  
  /* Set LPDS, MRLVDS and LPLVDS bits according to PWR_Regulator value */
  tmpreg |= PWR_Regulator;
  
  /* Store the new value */
  PWR->CR = tmpreg;
  
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  
  /* Select STOP mode entry --------------------------------------------------*/
  if(PWR_STOPEntry == PWR_STOPEntry_WFI)
  {   
    /* Request Wait For Interrupt */
    __WFI();
  }
  else
  {
    /* Request Wait For Event */
    __WFE();
  }
  /* Reset SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);  
}

/**
  * @brief  Enters in Under-Drive STOP mode.
  *  
  * @note   This mode is only available for STM32F42xxx/STM3243xxx devices. 
  * 
  * @note    This mode can be selected only when the Under-Drive is already active 
  *         
  * @note   In Stop mode, all I/O pins keep the same state as in Run mode.
  * @note   When exiting Stop mode by issuing an interrupt or a wakeup event, 
  *         the HSI RC oscillator is selected as system clock.
  * @note   When the voltage regulator operates in low power mode, an additional 
  *         startup delay is incurred when waking up from Stop mode. 
  *         By keeping the internal regulator ON during Stop mode, the consumption 
  *         is higher although the startup time is reduced.
  *     
  * @param  PWR_Regulator: specifies the regulator state in STOP mode.
  *          This parameter can be one of the following values:
  *            @arg PWR_MainRegulator_UnderDrive_ON:  Main Regulator in under-drive mode 
  *                 and Flash memory in power-down when the device is in Stop under-drive mode
  *            @arg PWR_LowPowerRegulator_UnderDrive_ON:  Low Power Regulator in under-drive mode 
  *                and Flash memory in power-down when the device is in Stop under-drive mode
  * @param  PWR_STOPEntry: specifies if STOP mode in entered with WFI or WFE instruction.
  *          This parameter can be one of the following values:
  *            @arg PWR_STOPEntry_WFI: enter STOP mode with WFI instruction
  *            @arg PWR_STOPEntry_WFE: enter STOP mode with WFE instruction
  * @retval None
  */
void PWR_EnterUnderDriveSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_PWR_REGULATOR_UNDERDRIVE(PWR_Regulator));
  assert_param(IS_PWR_STOP_ENTRY(PWR_STOPEntry));
  
  /* Select the regulator state in STOP mode ---------------------------------*/
  tmpreg = PWR->CR;
  /* Clear PDDS and LPDS bits */
  tmpreg &= CR_DS_MASK;
  
  /* Set LPDS, MRLUDS and LPLUDS bits according to PWR_Regulator value */
  tmpreg |= PWR_Regulator;
  
  /* Store the new value */
  PWR->CR = tmpreg;
  
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  
  /* Select STOP mode entry --------------------------------------------------*/
  if(PWR_STOPEntry == PWR_STOPEntry_WFI)
  {   
    /* Request Wait For Interrupt */
    __WFI();
  }
  else
  {
    /* Request Wait For Event */
    __WFE();
  }
  /* Reset SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);  
}

/**
  * @brief  Enters STANDBY mode.
  * @note   In Standby mode, all I/O pins are high impedance except for:
  *          - Reset pad (still available) 
  *          - RTC_AF1 pin (PC13) if configured for tamper, time-stamp, RTC 
  *            Alarm out, or RTC clock calibration out.
  *          - RTC_AF2 pin (PI8) if configured for tamper or time-stamp.  
  *          - WKUP pin 1 (PA0) if enabled.       
  * @param  None
  * @retval None
  */
void PWR_EnterSTANDBYMode(void)
{
  /* Clear Wakeup flag */
  PWR->CR |= PWR_CR_CWUF;
  
  /* Select STANDBY mode */
  PWR->CR |= PWR_CR_PDDS;
  
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  
/* This option is used to ensure that store operations are completed */
#if defined ( __CC_ARM   )
  __force_stores();
#endif
  /* Request Wait For Interrupt */
  __WFI();
}

/**
  * @}
  */

/** @defgroup PWR_Group7 Flags management functions
 *  @brief   Flags management functions 
 *
@verbatim   
 ===============================================================================
                    ##### Flags management functions #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Checks whether the specified PWR flag is set or not.
  * @param  PWR_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg PWR_FLAG_WU: Wake Up flag. This flag indicates that a wakeup event 
  *                  was received from the WKUP pin or from the RTC alarm (Alarm A 
  *                  or Alarm B), RTC Tamper event, RTC TimeStamp event or RTC Wakeup.
  *                  An additional wakeup event is detected if the WKUP pin is enabled 
  *                  (by setting the EWUP bit) when the WKUP pin level is already high.  
  *            @arg PWR_FLAG_SB: StandBy flag. This flag indicates that the system was
  *                  resumed from StandBy mode.    
  *            @arg PWR_FLAG_PVDO: PVD Output. This flag is valid only if PVD is enabled 
  *                  by the PWR_PVDCmd() function. The PVD is stopped by Standby mode 
  *                  For this reason, this bit is equal to 0 after Standby or reset
  *                  until the PVDE bit is set.
  *            @arg PWR_FLAG_BRR: Backup regulator ready flag. This bit is not reset 
  *                  when the device wakes up from Standby mode or by a system reset 
  *                  or power reset.  
  *            @arg PWR_FLAG_VOSRDY: This flag indicates that the Regulator voltage 
  *                 scaling output selection is ready.
  *            @arg PWR_FLAG_ODRDY: This flag indicates that the Over-drive mode
  *                 is ready (STM32F42xxx/43xxx devices) 
  *            @arg PWR_FLAG_ODSWRDY: This flag indicates that the Over-drive mode
  *                 switcching is ready (STM32F42xxx/43xxx devices) 
  *            @arg PWR_FLAG_UDRDY: This flag indicates that the Under-drive mode
  *                 is enabled in Stop mode (STM32F42xxx/43xxx devices)
  * @retval The new state of PWR_FLAG (SET or RESET).
  */
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG)
{
  FlagStatus bitstatus = RESET;
  
  /* Check the parameters */
  assert_param(IS_PWR_GET_FLAG(PWR_FLAG));
  
  if ((PWR->CSR & PWR_FLAG) != (uint32_t)RESET)
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
  * @brief  Clears the PWR's pending flags.
  * @param  PWR_FLAG: specifies the flag to clear.
  *          This parameter can be one of the following values:
  *            @arg PWR_FLAG_WU: Wake Up flag
  *            @arg PWR_FLAG_SB: StandBy flag
  *            @arg PWR_FLAG_UDRDY: Under-drive ready flag (STM32F42xxx/43xxx devices)
  * @retval None
  */
void PWR_ClearFlag(uint32_t PWR_FLAG)
{
  /* Check the parameters */
  assert_param(IS_PWR_CLEAR_FLAG(PWR_FLAG));
  
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
  if (PWR_FLAG != PWR_FLAG_UDRDY)
  {
    PWR->CR |=  PWR_FLAG << 2;
  }
  else
  {
    PWR->CSR |= PWR_FLAG_UDRDY;
  }
#endif /* STM32F427_437xx ||  STM32F429_439xx */

#if defined (STM32F40_41xxx) || defined (STM32F401xx) 
  PWR->CR |=  PWR_FLAG << 2;
#endif /* STM32F40_41xxx */
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
