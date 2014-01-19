/**
  ******************************************************************************
  * @file    stm32f4xx_syscfg.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    08-November-2013
  * @brief   This file provides firmware functions to manage the SYSCFG peripheral.
  *
 @verbatim
    
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..] This driver provides functions for:
            
       (#) Remapping the memory accessible in the code area using SYSCFG_MemoryRemapConfig()
            
       (#) Swapping the internal flash Bank1 and Bank2 this features is only visible for 
           STM32F42xxx/43xxx devices Devices. 
                
       (#) Manage the EXTI lines connection to the GPIOs using SYSCFG_EXTILineConfig()
              
       (#) Select the ETHERNET media interface (RMII/RII) using SYSCFG_ETH_MediaInterfaceConfig()
  
       -@- SYSCFG APB clock must be enabled to get write access to SYSCFG registers,
           using RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
                   
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
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup SYSCFG 
  * @brief SYSCFG driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* ------------ RCC registers bit address in the alias region ----------- */
#define SYSCFG_OFFSET             (SYSCFG_BASE - PERIPH_BASE)
/* ---  MEMRMP Register ---*/ 
/* Alias word address of UFB_MODE bit */ 
#define MEMRMP_OFFSET             SYSCFG_OFFSET 
#define UFB_MODE_BitNumber        ((uint8_t)0x8) 
#define UFB_MODE_BB               (PERIPH_BB_BASE + (MEMRMP_OFFSET * 32) + (UFB_MODE_BitNumber * 4)) 


/* ---  PMC Register ---*/ 
/* Alias word address of MII_RMII_SEL bit */ 
#define PMC_OFFSET                (SYSCFG_OFFSET + 0x04) 
#define MII_RMII_SEL_BitNumber    ((uint8_t)0x17) 
#define PMC_MII_RMII_SEL_BB       (PERIPH_BB_BASE + (PMC_OFFSET * 32) + (MII_RMII_SEL_BitNumber * 4)) 

/* ---  CMPCR Register ---*/ 
/* Alias word address of CMP_PD bit */ 
#define CMPCR_OFFSET              (SYSCFG_OFFSET + 0x20) 
#define CMP_PD_BitNumber          ((uint8_t)0x00) 
#define CMPCR_CMP_PD_BB           (PERIPH_BB_BASE + (CMPCR_OFFSET * 32) + (CMP_PD_BitNumber * 4)) 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup SYSCFG_Private_Functions
  * @{
  */ 

/**
  * @brief  Deinitializes the Alternate Functions (remap and EXTI configuration)
  *   registers to their default reset values.
  * @param  None
  * @retval None
  */
void SYSCFG_DeInit(void)
{
   RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, ENABLE);
   RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, DISABLE);
}

/**
  * @brief  Changes the mapping of the specified pin.
  * @param  SYSCFG_Memory: selects the memory remapping.
  *         This parameter can be one of the following values:
  *            @arg SYSCFG_MemoryRemap_Flash:       Main Flash memory mapped at 0x00000000  
  *            @arg SYSCFG_MemoryRemap_SystemFlash: System Flash memory mapped at 0x00000000
  *            @arg SYSCFG_MemoryRemap_FSMC:        FSMC (Bank1 (NOR/PSRAM 1 and 2) mapped at 0x00000000 for STM32F405xx/407xx and STM32F415xx/417xx devices. 
  *            @arg SYSCFG_MemoryRemap_FMC:         FMC (Bank1 (NOR/PSRAM 1 and 2) mapped at 0x00000000 for STM32F42xxx/43xxx devices.  
  *            @arg SYSCFG_MemoryRemap_SRAM:        Embedded SRAM (112kB) mapped at 0x00000000
  *            @arg SYSCFG_MemoryRemap_SDRAM:       FMC (External SDRAM)  mapped at 0x00000000 for STM32F42xxx/43xxx devices.            
  * @retval None
  */
void SYSCFG_MemoryRemapConfig(uint8_t SYSCFG_MemoryRemap)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_MEMORY_REMAP_CONFING(SYSCFG_MemoryRemap));

  SYSCFG->MEMRMP = SYSCFG_MemoryRemap;
}

/**
  * @brief  Enables or disables the Interal FLASH Bank Swapping.
  *   
  * @note   This function can be used only for STM32F42xxx/43xxx devices. 
  *
  * @param  NewState: new state of Interal FLASH Bank swapping.
  *          This parameter can be one of the following values:
  *            @arg ENABLE: Flash Bank2 mapped at 0x08000000 (and aliased @0x00000000) 
  *                         and Flash Bank1 mapped at 0x08100000 (and aliased at 0x00100000)   
  *            @arg DISABLE:(the default state) Flash Bank1 mapped at 0x08000000 (and aliased @0x0000 0000) 
                            and Flash Bank2 mapped at 0x08100000 (and aliased at 0x00100000)  
  * @retval None
  */
void SYSCFG_MemorySwappingBank(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  *(__IO uint32_t *) UFB_MODE_BB = (uint32_t)NewState;
}

/**
  * @brief  Selects the GPIO pin used as EXTI Line.
  * @param  EXTI_PortSourceGPIOx : selects the GPIO port to be used as source for
  *          EXTI lines where x can be (A..K) for STM32F42xxx/43xxx devices, (A..I) 
  *          for STM32F405xx/407xx and STM32F415xx/417xx devices or (A, B, C, D and H)
  *          for STM32401xx devices.  
  *            
  * @param  EXTI_PinSourcex: specifies the EXTI line to be configured.
  *           This parameter can be EXTI_PinSourcex where x can be (0..15, except
  *           for EXTI_PortSourceGPIOI x can be (0..11) for STM32F405xx/407xx
  *           and STM32F405xx/407xx devices and for EXTI_PortSourceGPIOK x can   
  *           be (0..7) for STM32F42xxx/43xxx devices. 
  *             
  * @retval None
  */
void SYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex)
{
  uint32_t tmp = 0x00;

  /* Check the parameters */
  assert_param(IS_EXTI_PORT_SOURCE(EXTI_PortSourceGPIOx));
  assert_param(IS_EXTI_PIN_SOURCE(EXTI_PinSourcex));

  tmp = ((uint32_t)0x0F) << (0x04 * (EXTI_PinSourcex & (uint8_t)0x03));
  SYSCFG->EXTICR[EXTI_PinSourcex >> 0x02] &= ~tmp;
  SYSCFG->EXTICR[EXTI_PinSourcex >> 0x02] |= (((uint32_t)EXTI_PortSourceGPIOx) << (0x04 * (EXTI_PinSourcex & (uint8_t)0x03)));
}

/**
  * @brief  Selects the ETHERNET media interface 
  * @param  SYSCFG_ETH_MediaInterface: specifies the Media Interface mode. 
  *          This parameter can be one of the following values: 
  *            @arg SYSCFG_ETH_MediaInterface_MII: MII mode selected
  *            @arg SYSCFG_ETH_MediaInterface_RMII: RMII mode selected 
  * @retval None 
  */
void SYSCFG_ETH_MediaInterfaceConfig(uint32_t SYSCFG_ETH_MediaInterface) 
{ 
  assert_param(IS_SYSCFG_ETH_MEDIA_INTERFACE(SYSCFG_ETH_MediaInterface)); 
  /* Configure MII_RMII selection bit */ 
  *(__IO uint32_t *) PMC_MII_RMII_SEL_BB = SYSCFG_ETH_MediaInterface; 
}

/**
  * @brief  Enables or disables the I/O Compensation Cell.
  * @note   The I/O compensation cell can be used only when the device supply
  *         voltage ranges from 2.4 to 3.6 V.  
  * @param  NewState: new state of the I/O Compensation Cell.
  *          This parameter can be one of the following values:
  *            @arg ENABLE: I/O compensation cell enabled  
  *            @arg DISABLE: I/O compensation cell power-down mode  
  * @retval None
  */
void SYSCFG_CompensationCellCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  *(__IO uint32_t *) CMPCR_CMP_PD_BB = (uint32_t)NewState;
}

/**
  * @brief  Checks whether the I/O Compensation Cell ready flag is set or not.
  * @param  None
  * @retval The new state of the I/O Compensation Cell ready flag (SET or RESET)
  */
FlagStatus SYSCFG_GetCompensationCellStatus(void)
{
  FlagStatus bitstatus = RESET;
    
  if ((SYSCFG->CMPCR & SYSCFG_CMPCR_READY ) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/   
