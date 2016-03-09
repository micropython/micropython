/**
  ******************************************************************************
  * @file    stm32l4xx_hal_comp.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   COMP HAL module driver.  
  *          This file provides firmware functions to manage the following 
  *          functionalities of the COMP peripheral:
  *           + Initialization and de-initialization functions
  *           + Start/Stop operation functions in polling mode.
  *           + Start/Stop operation functions in interrupt mode.
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *         
  @verbatim
================================================================================
          ##### COMP Peripheral features #####
================================================================================
           
  [..]       
      The STM32L4xx device family integrates two analog comparators COMP1 and COMP2:
      (#) The inverting input (input minus) and non inverting input (input plus) can be set to the GPIO pins
          listed in the Reference Manual "COMP function description" chapter.
  
      (#) The COMP output is not configurable by the HAL COMP as on STM32F3 and STM32F0 
          series: redirection to TIMER peripheral is to set with HAL TIM input remapping functions.
  
      (#) The COMP output level is available using HAL_COMP_GetOutputLevel()
          and available on GPIO pins listed in the Reference Manual "COMP function description" chapter.
  
      (#) The comparators COMP1 and COMP2 can be combined in window mode.
  
      (#) The comparators COMP1 and COMP2 have interrupt capability with wake-up
          from Sleep and Stop modes (through the EXTI controller):
          (++) COMP1 is internally connected to EXTI Line 21
          (++) COMP2 is internally connected to EXTI Line 22

          From the corresponding IRQ handler, the right interrupt source can be retrieved with the 
          macros __HAL_COMP_COMP1_EXTI_GET_FLAG() and __HAL_COMP_COMP2_EXTI_GET_FLAG().

          
            ##### How to use this driver #####
================================================================================
  [..]
      This driver provides functions to configure and program the Comparators of all STM32L4xx devices.

      To use the comparator, perform the following steps:
  
      (#)  Initialize the COMP low level resources by implementing the HAL_COMP_MspInit():
      (++) Configure the inverting and non-inverting comparator inputs in analog mode using HAL_GPIO_Init().
      (++) Configure the comparator output in alternate function mode using HAL_GPIO_Init() to map the comparator 
           output to the GPIO pin.
      (++) If required enable the COMP interrupt by configuring and enabling EXTI line in Interrupt mode and 
           selecting the desired sensitivity level using HAL_GPIO_Init() function. After that enable the comparator
           interrupt vector using HAL_NVIC_EnableIRQ() function.
  
      (#) Configure the comparator using HAL_COMP_Init() function:
      (++) Select the inverting input (input minus)
      (++) Select the non-inverting input (input plus)
      (++) Select the hysteresis
      (++) Select the blanking source
      (++) Select the output polarity  
      (++) Select the power mode
      (++) Select the window mode

      -@@- HAL_COMP_Init() calls internally __HAL_RCC_SYSCFG_CLK_ENABLE() in order
          to enable the comparator(s).

      (#) On-the-fly reconfiguration of comparator(s) may be done by calling again HAL_COMP_Init(
          function with new input parameter values; HAL_COMP_MspInit() function shall be adapted 
          to support multi configurations.

      (#) Enable the comparator using HAL_COMP_Start() or HAL_COMP_Start_IT() functions.
    
      (#) Use HAL_COMP_TriggerCallback() and/or HAL_COMP_GetOutputLevel() functions
          to manage comparator outputs (events and output level).

      (#) Disable the comparator using HAL_COMP_Stop() or HAL_COMP_Stop_IT() 
          function.

      (#) De-initialize the comparator using HAL_COMP_DeInit() function.

      (#) For safety purposes comparator(s) can be locked using HAL_COMP_Lock() function.
          Only a MCU reset can reset that protection.
      
  @endverbatim
  ******************************************************************************
  
      Table 1. COMP Inputs for the STM32L4xx devices
      +---------------------------------------------------------+     
      |                        |                | COMP1 | COMP2 | 
      |------------------------|----------------|---------------|
      |                        | 1/4 VREFINT    |  OK   |  OK   |  
      |                        | 1/2 VREFINT    |  OK   |  OK   |
      |                        | 3/4 VREFINT    |  OK   |  OK   |
      | Inverting Input        | VREFINT        |  OK   |  OK   | 
      | (minus)                | DAC1 OUT       |  OK   |  OK   |  
      |                        | DAC2 OUT       |  OK   |  OK   |  
      |                        | IO1            |  PB1  |  PB3  |  
      |                        | IO2            |  PC4  |  PB7  |  
      |------------------------|----------------|-------|-------|
      |  Non Inverting Input   | IO1            |  PC5  |  PB4  |  
      |  (plus)                | IO2            |  PB2  |  PB6  |
      +--------------------------------------------------+  
  
      Table 2. COMP Outputs for the STM32L4xx devices
      +------------------------------------+     
      |       COMP1      |      COMP2      | 
      |------------------|-----------------|
      |      PB0 (AF)    |     PB5 (AF)    | 
      |      PB10 (AF)   |     PB11 (AF)   | 
      |------------------|-----------------|
      |  Embedded TIMERS | Embedded TIMERS | 
      |  (cf. HAL TIM)   | (cf. HAL TIM)   | 
      +------------------------------------+

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

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @defgroup COMP COMP
  * @brief COMP HAL module driver
  * @{
  */

#ifdef HAL_COMP_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup COMP_Private_Constants
  * @{
  */
/* CSR register reset value */ 
#define COMP_CSR_RESET_VALUE               ((uint32_t)0x00000000)

/* CSR register Mask: all fields except read-only, lock and enable bits */ 
#define COMP_CSR_UPDATE_PARAMETERS_MASK    (COMP_CSR_PWRMODE  | COMP_CSR_INMSEL   | COMP_CSR_INPSEL  | \
                                            COMP_CSR_WINMODE  | COMP_CSR_POLARITY | COMP_CSR_HYST    | \
                                            COMP_CSR_BLANKING | COMP_CSR_BRGEN    | COMP_CSR_SCALEN)

#define COMP_LOCK_DISABLE                   ((uint32_t)0x00000000)
#define COMP_LOCK_ENABLE                    COMP_CSR_LOCK

#define COMP_STATE_BIT_LOCK                 ((uint8_t)0x10)

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup COMP_Exported_Functions COMP Exported Functions
  * @{
  */

/** @defgroup COMP_Exported_Functions_Group1 Initialization/de-initialization functions 
 *  @brief    Initialization and de-initialization functions. 
 *
@verbatim    
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions to initialize and de-initialize comparators 

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the COMP according to the specified
  *         parameters in the COMP_InitTypeDef and initialize the associated handle.
  * @note   If the selected comparator is locked, initialization can't be performed.
  *         To unlock the configuration, perform a system reset.
  * @param  hcomp  COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_Init(COMP_HandleTypeDef *hcomp)
{
  uint32_t          tmpcsr = 0;
  HAL_StatusTypeDef status = HAL_OK;
  
  /* Check the COMP handle allocation and lock status */
  if((hcomp == NULL) || ((hcomp->State & COMP_STATE_BIT_LOCK) != RESET))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameters */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));
    assert_param(IS_COMP_INVERTINGINPUT(hcomp->Init.InvertingInput));
    assert_param(IS_COMP_NONINVERTINGINPUT(hcomp->Init.NonInvertingInput));
    assert_param(IS_COMP_OUTPUTPOL(hcomp->Init.OutputPol));
    assert_param(IS_COMP_MODE(hcomp->Init.Mode));
    assert_param(IS_COMP_HYSTERESIS(hcomp->Init.Hysteresis));
    assert_param(IS_COMP_BLANKINGSRCE(hcomp->Init.BlankingSrce)); 
    assert_param(IS_COMP_BLANKINGSRCE_INSTANCE(hcomp->Instance, hcomp->Init.BlankingSrce)); 
    assert_param(IS_COMP_TRIGGERMODE(hcomp->Init.TriggerMode));

    if(hcomp->Init.WindowMode != COMP_WINDOWMODE_DISABLE)
    {
      assert_param(IS_COMP_WINDOWMODE_INSTANCE(hcomp->Instance));
      assert_param(IS_COMP_WINDOWMODE(hcomp->Init.WindowMode));
    }

    /* Init SYSCFG and the low level hardware to access comparators */
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    /* Init the low level hardware : SYSCFG to access comparators */
    HAL_COMP_MspInit(hcomp);
    
    if(hcomp->State == HAL_COMP_STATE_RESET)
    {
      /* Allocate lock resource and initialize it */
      hcomp->Lock = HAL_UNLOCKED;
    }

    /* Set COMP parameters */
    /*     Set INMSEL bits according to hcomp->Init.InvertingInput value       */
    /*     Set INPSEL bits according to hcomp->Init.NonInvertingInput value    */
    /*     Set BLANKING bits according to hcomp->Init.BlankingSrce value       */
    /*     Set HYST bits according to hcomp->Init.Hysteresis value             */
    /*     Set POLARITY bit according to hcomp->Init.OutputPol value           */
    /*     Set POWERMODE bits according to hcomp->Init.Mode value              */
    /*     Set WINMODE bit according to hcomp->Init.WindowMode value           */
    tmpcsr = hcomp->Init.InvertingInput    |  \
             hcomp->Init.NonInvertingInput |  \
             hcomp->Init.BlankingSrce      |  \
             hcomp->Init.Hysteresis        |  \
             hcomp->Init.OutputPol         |  \
             hcomp->Init.Mode              |  \
             hcomp->Init.WindowMode;
    
    /* Check VREFINT use for NonInvertingInput */
    if(hcomp->Init.InvertingInput == COMP_INVERTINGINPUT_VREFINT)
    {
      /* Enable voltage scaler to output VREFINT */
      tmpcsr |= COMP_CSR_SCALEN;
    }
    else
    {
      if((hcomp->Init.InvertingInput == COMP_INVERTINGINPUT_1_4VREFINT) ||
         (hcomp->Init.InvertingInput == COMP_INVERTINGINPUT_1_2VREFINT) ||
         (hcomp->Init.InvertingInput == COMP_INVERTINGINPUT_3_4VREFINT))
      {
        /* Enable voltage & bandgap scaler to output VREFINT */
        tmpcsr |= (COMP_CSR_BRGEN | COMP_CSR_SCALEN);
      }
    }
    
    MODIFY_REG(hcomp->Instance->CSR, COMP_CSR_UPDATE_PARAMETERS_MASK, tmpcsr);

    /* Update the COMP state*/
    if (hcomp->State == HAL_COMP_STATE_RESET)
    {
      /* From RESET state to READY State */
      hcomp->State = HAL_COMP_STATE_READY;
    }
    /* else: remain in READY or BUSY state (no update) */
  }
  
  return status;
}

/**
  * @brief  DeInitialize the COMP peripheral.
  * @note   Deinitialization cannot be performed if the COMP configuration is locked.
  *         To unlock the configuration, perform a system reset.
  * @param  hcomp  COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_DeInit(COMP_HandleTypeDef *hcomp)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the COMP handle allocation and lock status */
  if((hcomp == NULL) || ((hcomp->State & COMP_STATE_BIT_LOCK) != RESET))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

    /* Set COMP_CSR register to reset value */
    WRITE_REG(hcomp->Instance->CSR, COMP_CSR_RESET_VALUE);

    /* DeInit the low level hardware: SYSCFG, GPIO, CLOCK and NVIC */
    HAL_COMP_MspDeInit(hcomp);

    hcomp->State = HAL_COMP_STATE_RESET;
    
    /* Release Lock */
    __HAL_UNLOCK(hcomp);
  }
  
  return status;
}

/**
  * @brief  Initialize the COMP MSP.
  * @param  hcomp  COMP handle
  * @retval None
  */
__weak void HAL_COMP_MspInit(COMP_HandleTypeDef *hcomp)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcomp);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_COMP_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitialize the COMP MSP.
  * @param  hcomp  COMP handle
  * @retval None
  */
__weak void HAL_COMP_MspDeInit(COMP_HandleTypeDef *hcomp)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcomp);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_COMP_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup COMP_Exported_Functions_Group2 Start-Stop operation functions 
 *  @brief   Start-Stop operation functions. 
 *
@verbatim   
 ===============================================================================
                      ##### COMP Start Stop operation functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Start a Comparator without interrupt.
      (+) Stop a Comparator without interrupt.
      (+) Start a Comparator with interrupt generation.
      (+) Stop a Comparator with interrupt generation.

@endverbatim
  * @{
  */

/**
  * @brief  Start the comparator.
  * @param  hcomp  COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_Start(COMP_HandleTypeDef *hcomp)
{ 
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t extiline = 0;
  
  /* Check the COMP handle allocation and lock status */
  if((hcomp == NULL) || ((hcomp->State & COMP_STATE_BIT_LOCK) != RESET))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

    if(hcomp->State == HAL_COMP_STATE_READY)
    {
      /* Get the EXTI Line output configuration */
      extiline = COMP_GET_EXTI_LINE(hcomp->Instance);

      /* Configure the event generation */
      if((hcomp->Init.TriggerMode & (COMP_TRIGGERMODE_EVENT_RISING|COMP_TRIGGERMODE_EVENT_FALLING)) != RESET)
      {
        /* Configure the event trigger rising edge */
        if((hcomp->Init.TriggerMode & COMP_TRIGGERMODE_EVENT_RISING) != RESET)
        {
          SET_BIT(EXTI->RTSR1, extiline);
        }
        else
        {
          CLEAR_BIT(EXTI->RTSR1, extiline);
        }

        /* Configure the trigger falling edge */
        if((hcomp->Init.TriggerMode & COMP_TRIGGERMODE_EVENT_FALLING) != RESET)
        {
          SET_BIT(EXTI->FTSR1, extiline);
        }
        else
        {
          CLEAR_BIT(EXTI->FTSR1, extiline);
        }

        /* Clear COMP EXTI pending bit if any */
        WRITE_REG(EXTI->PR1, extiline);

        /* Enable EXTI event generation */
        SET_BIT(EXTI->EMR1, extiline);
      }
      else
      {
        /* Make sur EXTI event generation is disabled */
        CLEAR_BIT(EXTI->EMR1, extiline);
      }

      /* Enable the selected comparator */
      SET_BIT(hcomp->Instance->CSR, COMP_CSR_EN);

      hcomp->State = HAL_COMP_STATE_BUSY;
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  Stop the comparator.
  * @param  hcomp  COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_Stop(COMP_HandleTypeDef *hcomp)
{ 
  HAL_StatusTypeDef status = HAL_OK;
  
  /* Check the COMP handle allocation and lock status */
  if((hcomp == NULL) || ((hcomp->State & COMP_STATE_BIT_LOCK) != RESET))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

    if(hcomp->State == HAL_COMP_STATE_BUSY)
    {
      /* Disable the EXTI Line event mode if any */
      CLEAR_BIT(EXTI->EMR1, COMP_GET_EXTI_LINE(hcomp->Instance));

      /* Disable the selected comparator */
      CLEAR_BIT(hcomp->Instance->CSR, COMP_CSR_EN);

      hcomp->State = HAL_COMP_STATE_READY;
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  
  return status;
}

/**
  * @brief  Start the comparator in Interrupt mode.
  * @param  hcomp  COMP handle
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_COMP_Start_IT(COMP_HandleTypeDef *hcomp)
{ 
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t extiline = 0;
  
  /* Check the COMP handle allocation and lock status */
  if((hcomp == NULL) || ((hcomp->State & COMP_STATE_BIT_LOCK) != RESET))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

    if(hcomp->State == HAL_COMP_STATE_READY)
    {
      /* Configure the EXTI event generation */
      if((hcomp->Init.TriggerMode & (COMP_TRIGGERMODE_IT_RISING|COMP_TRIGGERMODE_IT_FALLING)) != RESET)
      {
        /* Get the EXTI Line output configuration */
        extiline = COMP_GET_EXTI_LINE(hcomp->Instance);

        /* Configure the trigger rising edge */
        if((hcomp->Init.TriggerMode & COMP_TRIGGERMODE_IT_RISING) != RESET)
        {
          SET_BIT(EXTI->RTSR1, extiline);
        }
        else
        {
          CLEAR_BIT(EXTI->RTSR1, extiline);
        }
        /* Configure the trigger falling edge */
        if((hcomp->Init.TriggerMode & COMP_TRIGGERMODE_IT_FALLING) != RESET)
        {
          SET_BIT(EXTI->FTSR1, extiline);
        }
        else
        {
          CLEAR_BIT(EXTI->FTSR1, extiline);
        }

        /* Clear COMP EXTI pending bit if any */
        WRITE_REG(EXTI->PR1, extiline);

        /* Enable EXTI interrupt mode */
        SET_BIT(EXTI->IMR1, extiline);

        /* Enable the selected comparator */
        SET_BIT(hcomp->Instance->CSR, COMP_CSR_EN);

        hcomp->State = HAL_COMP_STATE_BUSY;
      }
      else
      {
        status = HAL_ERROR;
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  Stop the comparator in Interrupt mode.
  * @param  hcomp  COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_Stop_IT(COMP_HandleTypeDef *hcomp)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  /* Disable the EXTI Line interrupt mode */
  CLEAR_BIT(EXTI->IMR1, COMP_GET_EXTI_LINE(hcomp->Instance));
  
  status = HAL_COMP_Stop(hcomp);
  
  return status;
}

/**
  * @brief  Comparator IRQ Handler.
  * @param  hcomp  COMP handle
  * @retval None
  */
void HAL_COMP_IRQHandler(COMP_HandleTypeDef *hcomp)
{
  uint32_t extiline = COMP_GET_EXTI_LINE(hcomp->Instance);
  
  /* Check COMP EXTI flag */
  if(READ_BIT(EXTI->PR1, extiline) != RESET)
  {
    /* Clear COMP EXTI pending bit */
    WRITE_REG(EXTI->PR1, extiline);
  
    /* COMP trigger user callback */
    HAL_COMP_TriggerCallback(hcomp);
  }
}

/**
  * @}
  */

/** @defgroup COMP_Exported_Functions_Group3 Peripheral Control functions 
 *  @brief   Management functions.
 *
@verbatim   
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to control the comparators. 

@endverbatim
  * @{
  */

/**
  * @brief  Lock the selected comparator configuration.
  * @note   A system reset is required to unlock the comparator configuration.
  * @note   Locking the comparator from its reset state is possible provided
  *         __HAL_RCC_SYSCFG_CLK_ENABLE() is being called before.
  * @param  hcomp  COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_Lock(COMP_HandleTypeDef *hcomp)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the COMP handle allocation and lock status */
  if((hcomp == NULL) || ((hcomp->State & COMP_STATE_BIT_LOCK) != RESET))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

    hcomp->State = ((HAL_COMP_StateTypeDef)(hcomp->State | COMP_STATE_BIT_LOCK));
  }
  
  if(status == HAL_OK)
  {
    /* Set the lock bit corresponding to selected comparator */
    __HAL_COMP_LOCK(hcomp);
  }
  
  return status; 
}

/**
  * @brief  Return the output level (high or low) of the selected comparator. 
  *         The output level depends on the selected polarity.
  *         If the polarity is not inverted:
  *           - Comparator output is low when the non-inverting input is at a lower
  *             voltage than the inverting input
  *           - Comparator output is high when the non-inverting input is at a higher
  *             voltage than the inverting input
  *         If the polarity is inverted:
  *           - Comparator output is high when the non-inverting input is at a lower
  *             voltage than the inverting input
  *           - Comparator output is low when the non-inverting input is at a higher
  *             voltage than the inverting input
  * @param  hcomp  COMP handle
  * @retval Returns the selected comparator output level: 
  *         @arg @ref COMP_OUTPUTLEVEL_LOW
  *         @arg @ref COMP_OUTPUTLEVEL_HIGH
  *       
  */
uint32_t HAL_COMP_GetOutputLevel(COMP_HandleTypeDef *hcomp)
{
  /* Check the parameter */
  assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));
  
  return((uint32_t)(hcomp->Instance->CSR & COMP_OUTPUTLEVEL_HIGH));
}

/**
  * @brief  Comparator callback.
  * @param  hcomp  COMP handle
  * @retval None
  */
__weak void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcomp);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_COMP_TriggerCallback should be implemented in the user file
   */
}


/**
  * @}
  */

/** @defgroup COMP_Exported_Functions_Group4 Peripheral State functions 
 *  @brief   Peripheral State functions. 
 *
@verbatim   
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================  
    [..]
    This subsection permits to get in run-time the status of the peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Return the COMP handle state.
  * @param  hcomp  COMP handle
  * @retval HAL state
  */
HAL_COMP_StateTypeDef HAL_COMP_GetState(COMP_HandleTypeDef *hcomp)
{
  /* Check the COMP handle allocation */
  if(hcomp == NULL)
  {
    return HAL_COMP_STATE_RESET;
  }

  /* Check the parameter */
  assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

  /* Return COMP handle state */
  return hcomp->State;
}
/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_COMP_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
