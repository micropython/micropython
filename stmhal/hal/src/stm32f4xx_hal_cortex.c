/**
  ******************************************************************************
  * @file    stm32f4xx_hal_cortex.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   CORTEX HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the CORTEX:
  *           + Initialization and de-initialization functions
  *           + Peripheral Control functions 
  *
  @verbatim  
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================

    [..]  
    *** How to configure Interrupts using Cortex HAL driver ***
    ===========================================================
    [..]     
    This section provide functions allowing to configure the NVIC interrupts (IRQ).
    The Cortex-M4 exceptions are managed by CMSIS functions.
   
    (#) Configure the NVIC Priority Grouping using HAL_NVIC_SetPriorityGrouping()
        function according to the following table.
        
     The table below gives the allowed values of the pre-emption priority and subpriority according
     to the Priority Grouping configuration performed by HAL_NVIC_SetPriorityGrouping() function.
       ==========================================================================================================================
         NVIC_PriorityGroup   | NVIC_IRQChannelPreemptionPriority | NVIC_IRQChannelSubPriority  |       Description
       ==========================================================================================================================
        NVIC_PRIORITYGROUP_0  |                0                  |            0-15             | 0 bits for pre-emption priority
                              |                                   |                             | 4 bits for subpriority
       --------------------------------------------------------------------------------------------------------------------------
        NVIC_PRIORITYGROUP_1  |                0-1                |            0-7              | 1 bits for pre-emption priority
                              |                                   |                             | 3 bits for subpriority
       --------------------------------------------------------------------------------------------------------------------------    
        NVIC_PRIORITYGROUP_2  |                0-3                |            0-3              | 2 bits for pre-emption priority
                              |                                   |                             | 2 bits for subpriority
       --------------------------------------------------------------------------------------------------------------------------    
        NVIC_PRIORITYGROUP_3  |                0-7                |            0-1              | 3 bits for pre-emption priority
                              |                                   |                             | 1 bits for subpriority
       --------------------------------------------------------------------------------------------------------------------------    
        NVIC_PRIORITYGROUP_4  |                0-15               |            0                | 4 bits for pre-emption priority
                              |                                   |                             | 0 bits for subpriority                       
       ==========================================================================================================================
     (#)  Configure the priority of the selected IRQ Channels using HAL_NVIC_SetPriority() 

     (#)  Enable the selected IRQ Channels using HAL_NVIC_EnableIRQ() 
      

     -@- When the NVIC_PRIORITYGROUP_0 is selected, IRQ pre-emption is no more possible. 
         The pending IRQ priority will be managed only by the sub priority.
   
     -@- IRQ priority order (sorted by highest to lowest priority):
        (+@) Lowest pre-emption priority
        (+@) Lowest sub priority
        (+@) Lowest hardware priority (IRQ number)
 
    [..]  
    *** How to configure Systick using Cortex HAL driver ***
    ========================================================
    [..]
    Setup SysTick Timer for 1 msec interrupts.
           
   (+) The HAL_SYSTICK_Config()function calls the SysTick_Config() function which
       is a CMSIS function that:
        (++) Configures the SysTick Reload register with value passed as function parameter.
        (++) Configures the SysTick IRQ priority to the lowest value (0x0F).
        (++) Resets the SysTick Counter register.
        (++) Configures the SysTick Counter clock source to be Core Clock Source (HCLK).
        (++) Enables the SysTick Interrupt.
        (++) Starts the SysTick Counter.
    
   (+) You can change the SysTick Clock source to be HCLK_Div8 by calling the macro
       __HAL_CORTEX_SYSTICKCLK_CONFIG(SYSTICK_CLKSOURCE_HCLK_DIV8) just after the
       HAL_SYSTICK_Config() function call. The __HAL_CORTEX_SYSTICKCLK_CONFIG() macro is defined
       inside the stm32f4xx_hal_cortex.h file.

   (+) You can change the SysTick IRQ priority by calling the
       HAL_NVIC_SetPriority(SysTick_IRQn,...) function just after the HAL_SYSTICK_Config() function 
       call. The HAL_NVIC_SetPriority() call the NVIC_SetPriority() function which is a CMSIS function.

   (+) To adjust the SysTick time base, use the following formula:
                            
       Reload Value = SysTick Counter Clock (Hz) x  Desired Time base (s)
       (++) Reload Value is the parameter to be passed for HAL_SYSTICK_Config() function
       (++) Reload Value should not exceed 0xFFFFFF
   
  @endverbatim
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

/** @defgroup CORTEX 
  * @brief CORTEX HAL module driver
  * @{
  */

#ifdef HAL_CORTEX_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CORTEX_Private_Functions
  * @{
  */


/** @defgroup CORTEX_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
  ==============================================================================
              ##### Initialization and de-initialization functions #####
  ==============================================================================
    [..]
      This section provide the Cortex HAL driver functions allowing to configure Interrupts
      Systick functionalities 

@endverbatim
  * @{
  */


/**
  * @brief  Sets the priority grouping field (pre-emption priority and subpriority)
  *         using the required unlock sequence.
  * @param  PriorityGroup: The priority grouping bits length. 
  *         This parameter can be one of the following values:
  *         @arg NVIC_PRIORITYGROUP_0: 0 bits for pre-emption priority
  *                                    4 bits for subpriority
  *         @arg NVIC_PRIORITYGROUP_1: 1 bits for pre-emption priority
  *                                    3 bits for subpriority
  *         @arg NVIC_PRIORITYGROUP_2: 2 bits for pre-emption priority
  *                                    2 bits for subpriority
  *         @arg NVIC_PRIORITYGROUP_3: 3 bits for pre-emption priority
  *                                    1 bits for subpriority
  *         @arg NVIC_PRIORITYGROUP_4: 4 bits for pre-emption priority
  *                                    0 bits for subpriority
  * @note   When the NVIC_PriorityGroup_0 is selected, IRQ pre-emption is no more possible. 
  *         The pending IRQ priority will be managed only by the subpriority. 
  * @retval None
  */
void HAL_NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
  /* Check the parameters */
  assert_param(IS_NVIC_PRIORITY_GROUP(PriorityGroup));
  
  /* Set the PRIGROUP[10:8] bits according to the PriorityGroup parameter value */
  NVIC_SetPriorityGrouping(PriorityGroup);
}

/**
  * @brief  Sets the priority of an interrupt.
  * @param  IRQn: External interrupt number
  *         This parameter can be an enumerator of @ref IRQn_Type enumeration
  *         (For the complete STM32 Devices IRQ Channels list, please refer to stm32f4xx.h file)
  * @param  PreemptPriority: The pre-emption priority for the IRQn channel.
  *         This parameter can be a value between 0 and 15
  *         A lower priority value indicates a higher priority 
  * @param  SubPriority: the subpriority level for the IRQ channel.
  *         This parameter can be a value between 0 and 15
  *         A lower priority value indicates a higher priority.          
  * @retval None
  */
void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{ 
  uint32_t prioritygroup = 0x00;
  
  /* Check the parameters */
  assert_param(IS_NVIC_SUB_PRIORITY(SubPriority));
  assert_param(IS_NVIC_PREEMPTION_PRIORITY(PreemptPriority));
  
  prioritygroup = NVIC_GetPriorityGrouping();
  
  NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
}

/**
  * @brief  Enables a device specific interrupt in the NVIC interrupt controller.
  * @note   To configure interrupts priority correctly, the NVIC_PriorityGroupConfig()
  *         function should be called before. 
  * @param  IRQn External interrupt number
  *         This parameter can be an enumerator of @ref IRQn_Type enumeration
  *         (For the complete STM32 Devices IRQ Channels list, please refer to stm32f4xx.h file)  
  * @retval None
  */
void HAL_NVIC_EnableIRQ(IRQn_Type IRQn)
{
  /* Enable interrupt */
  NVIC_EnableIRQ(IRQn);
}

/**
  * @brief  Disables a device specific interrupt in the NVIC interrupt controller.
  * @param  IRQn External interrupt number
  *         This parameter can be an enumerator of @ref IRQn_Type enumeration
  *         (For the complete STM32 Devices IRQ Channels list, please refer to stm32f4xx.h file)  
  * @retval None
  */
void HAL_NVIC_DisableIRQ(IRQn_Type IRQn)
{
  /* Disable interrupt */
  NVIC_DisableIRQ(IRQn);
}

/**
  * @brief  Initiates a system reset request to reset the MCU.
  * @param None
  * @retval None
  */
void HAL_NVIC_SystemReset(void)
{
  /* System Reset */
  NVIC_SystemReset();
}

/**
  * @brief  Initializes the System Timer and its interrupt, and starts the System Tick Timer.
  *         Counter is in free running mode to generate periodic interrupts.
  * @param  TicksNumb: Specifies the ticks Number of ticks between two interrupts.
  * @retval status:  - 0  Function succeeded.
  *                  - 1  Function failed.
  */
uint32_t HAL_SYSTICK_Config(uint32_t TicksNumb)
{
   return SysTick_Config(TicksNumb);
}
/**
  * @}
  */

/** @defgroup CORTEX_Group2 Peripheral Control functions 
 *  @brief   Cortex control functions 
 *
@verbatim   
  ==============================================================================
                      ##### Peripheral Control functions #####
  ==============================================================================  
    [..]
      This subsection provides a set of functions allowing to control the CORTEX
      (NVIC, SYSTICK) functionalities. 
 
      
@endverbatim
  * @{
  */

/**
  * @brief  Gets the priority grouping field from the NVIC Interrupt Controller.
  * @param  None
  * @retval Priority grouping field (SCB->AIRCR [10:8] PRIGROUP field)
  */
uint32_t HAL_NVIC_GetPriorityGrouping(void)
{
  /* Get the PRIGROUP[10:8] field value */
  return NVIC_GetPriorityGrouping();
}

/**
  * @brief  Gets the priority of an interrupt.
  * @param  IRQn: External interrupt number
  *         This parameter can be an enumerator of @ref IRQn_Type enumeration
  *         (For the complete STM32 Devices IRQ Channels list, please refer to stm32f4xx.h file)
  * @param   PriorityGroup: the priority grouping bits length.
  *         This parameter can be one of the following values:
  *           @arg NVIC_PRIORITYGROUP_0: 0 bits for pre-emption priority
  *                                      4 bits for subpriority
  *           @arg NVIC_PRIORITYGROUP_1: 1 bits for pre-emption priority
  *                                      3 bits for subpriority
  *           @arg NVIC_PRIORITYGROUP_2: 2 bits for pre-emption priority
  *                                      2 bits for subpriority
  *           @arg NVIC_PRIORITYGROUP_3: 3 bits for pre-emption priority
  *                                      1 bits for subpriority
  *           @arg NVIC_PRIORITYGROUP_4: 4 bits for pre-emption priority
  *                                      0 bits for subpriority
  * @param  pPreemptPriority: Pointer on the Preemptive priority value (starting from 0).
  * @param  pSubPriority: Pointer on the Subpriority value (starting from 0).
  * @retval None
  */
void HAL_NVIC_GetPriority(IRQn_Type IRQn, uint32_t PriorityGroup, uint32_t *pPreemptPriority, uint32_t *pSubPriority)
{
  /* Check the parameters */
  assert_param(IS_NVIC_PRIORITY_GROUP(PriorityGroup));
 /* Get priority for Cortex-M system or device specific interrupts */
  NVIC_DecodePriority(NVIC_GetPriority(IRQn), PriorityGroup, pPreemptPriority, pSubPriority);
}

/**
  * @brief  Sets Pending bit of an external interrupt.
  * @param  IRQn External interrupt number
  *         This parameter can be an enumerator of @ref IRQn_Type enumeration
  *         (For the complete STM32 Devices IRQ Channels list, please refer to stm32f4xx.h file)  
  * @retval None
  */
void HAL_NVIC_SetPendingIRQ(IRQn_Type IRQn)
{ 
  /* Set interrupt pending */
  NVIC_SetPendingIRQ(IRQn);
}

/**
  * @brief  Gets Pending Interrupt (reads the pending register in the NVIC 
  *         and returns the pending bit for the specified interrupt).
  * @param  IRQn External interrupt number
  *          This parameter can be an enumerator of @ref IRQn_Type enumeration
  *          (For the complete STM32 Devices IRQ Channels list, please refer to stm32f4xx.h file)  
  * @retval status: - 0  Interrupt status is not pending.
  *                 - 1  Interrupt status is pending.
  */
uint32_t HAL_NVIC_GetPendingIRQ(IRQn_Type IRQn)
{ 
  /* Return 1 if pending else 0 */
  return NVIC_GetPendingIRQ(IRQn);
}

/**
  * @brief  Clears the pending bit of an external interrupt.
  * @param  IRQn External interrupt number
  *         This parameter can be an enumerator of @ref IRQn_Type enumeration
  *         (For the complete STM32 Devices IRQ Channels list, please refer to stm32f4xx.h file)  
  * @retval None
  */
void HAL_NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{ 
  /* Clear pending interrupt */
  NVIC_ClearPendingIRQ(IRQn);
}

/**
  * @brief Gets active interrupt ( reads the active register in NVIC and returns the active bit).
  * @param IRQn External interrupt number
  *         This parameter can be an enumerator of @ref IRQn_Type enumeration
  *         (For the complete STM32 Devices IRQ Channels list, please refer to stm32f4xx.h file)  
  * @retval status: - 0  Interrupt status is not pending.
  *                 - 1  Interrupt status is pending.
  */
uint32_t HAL_NVIC_GetActive(IRQn_Type IRQn)
{ 
  /* Return 1 if active else 0 */
  return NVIC_GetActive(IRQn);
}

/**
  * @brief  Configures the SysTick clock source.
  * @param  CLKSource: specifies the SysTick clock source.
  *          This parameter can be one of the following values:
  *             @arg SYSTICK_CLKSOURCE_HCLK_DIV8: AHB clock divided by 8 selected as SysTick clock source.
  *             @arg SYSTICK_CLKSOURCE_HCLK: AHB clock selected as SysTick clock source.
  * @retval None
  */
void HAL_SYSTICK_CLKSourceConfig(uint32_t CLKSource)
{
  /* Check the parameters */
  assert_param(IS_SYSTICK_CLK_SOURCE(CLKSource));
  if (CLKSource == SYSTICK_CLKSOURCE_HCLK)
  {
    SysTick->CTRL |= SYSTICK_CLKSOURCE_HCLK;
  }
  else
  {
    SysTick->CTRL &= ~SYSTICK_CLKSOURCE_HCLK;
  }
}

/**
  * @brief  This function handles SYSTICK interrupt request.
  * @param  None
  * @retval None
  */
void HAL_SYSTICK_IRQHandler(void)
{
  HAL_SYSTICK_Callback();
}

/**
  * @brief  SYSTICK callback.
  * @param  None
  * @retval None
  */
__weak void HAL_SYSTICK_Callback(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SYSTICK_Callback could be implemented in the user file
   */
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_CORTEX_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
