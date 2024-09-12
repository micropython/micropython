/**
  ******************************************************************************
  * @file    stm32n6xx_hal_gpio.c
  * @author  GPM Application Team
  * @brief   GPIO HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the General Purpose Input/Output (GPIO) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                    ##### GPIO Peripheral features #####
  ==============================================================================
  [..]
    (+) Each port bit of the general-purpose I/O (GPIO) ports can be individually
        configured by software in several modes:
        (++) Input mode
        (++) Analog mode
        (++) Output mode
        (++) Alternate function mode
        (++) External interrupt/event lines

    (+) During and just after reset, the alternate functions and external interrupt
        lines are not active and the I/O ports are configured in analog mode.

    (+) All GPIO pins have weak internal pull-up and pull-down resistors, which can be
        activated or not.

    (+) In Output or Alternate mode, each IO can be configured on open-drain or push-pull
        type and the IO speed can be selected depending on the VDD value.

    (+) The microcontroller IO pins are connected to onboard peripherals/modules through a
        multiplexer that allows only one peripheral alternate function (AF) connected
        to an IO pin at a time. In this way, there can be no conflict between peripherals
        sharing the same IO pin.

    (+) All ports have external interrupt/event capability. To use external interrupt
        lines, the port must be configured in input mode. All available GPIO pins are
        connected to the 16 external interrupt/event lines from EXTI0 to EXTI15.

    (+) The external interrupt/event controller consists of up to 73 edge detectors
        (16 lines are connected to GPIO) for generating event/interrupt requests (each
        input line can be independently configured to select the type (interrupt or event)
        and the corresponding trigger event (rising or falling or both). Each line can
        also be masked independently.

                     ##### How to use this driver #####
  ==============================================================================
  [..]
    (#) Enable the GPIO AHB clock using the following function: __HAL_RCC_GPIOx_CLK_ENABLE().

    (#) Configure the GPIO pin(s) using HAL_GPIO_Init().
        (++) Configure the IO mode using "Mode" member from GPIO_InitTypeDef structure
        (++) Activate Pull-up, Pull-down resistor using "Pull" member from GPIO_InitTypeDef
             structure.
        (++) In case of Output or alternate function mode selection: the speed is
             configured through "Speed" member from GPIO_InitTypeDef structure.
        (++) In alternate mode is selection, the alternate function connected to the IO
             is configured through "Alternate" member from GPIO_InitTypeDef structure.
        (++) Analog mode is required when a pin is to be used as ADC channel
             or DAC output.
        (++) In case of external interrupt/event selection the "Mode" member from
             GPIO_InitTypeDef structure select the type (interrupt or event) and
             the corresponding trigger event (rising or falling or both).

    (#) In case of external interrupt/event mode selection, configure NVIC IRQ priority
        mapped to the EXTI line using HAL_NVIC_SetPriority() and enable it using
        HAL_NVIC_EnableIRQ().

    (#) To get the level of a pin configured in input mode use HAL_GPIO_ReadPin().

    (#) To set/reset the level of a pin configured in output mode use
        HAL_GPIO_WritePin()/HAL_GPIO_TogglePin().

    (#) To set the level of several pins and reset level of several other pins in
        same cycle, use HAL_GPIO_WriteMultipleStatePin().

    (#) To lock pin configuration until next reset use HAL_GPIO_LockPin().

    (#) During and just after reset, the alternate functions are not
        active and the GPIO pins are configured in analog mode (except JTAG
        pins).

    (#) The LSE oscillator pins OSC32_IN and OSC32_OUT can be used as general purpose
        (PC14 and PC15, respectively) when the LSE oscillator is off. The LSE has
        priority over the GPIO function.

    (#) The HSE oscillator pins OSC_IN/OSC_OUT can be used as
        general purpose PH0 and PH1, respectively, when the HSE oscillator is off.
        The HSE has priority over the GPIO function.

  @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup GPIO
  * @{
  */
/** MISRA C:2012 deviation rule has been granted for following rules:
  * Rule-12.2 - Medium: RHS argument is in interval [0,INF] which is out of
  * range of the shift operator in following API :
  * HAL_GPIO_Init
  * HAL_GPIO_DeInit
  */

#ifdef HAL_GPIO_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private defines ------------------------------------------------------------*/
/** @addtogroup GPIO_Private_Constants
  * @{
  */
#define GPIO_NUMBER           (16u)
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup GPIO_Exported_Functions
  * @{
  */

/** @addtogroup GPIO_Exported_Functions_Group1
  *  @brief    Initialization and Configuration functions
  *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the GPIOx peripheral according to the specified parameters in the GPIO_Init.
  * @note   If GPIOx peripheral pin is used in EXTI_MODE and the pin is secure/privilege, it is up
  *         to the application to insure that the corresponding EXTI line is set secure/privilege.
  * @param  GPIOx where x can be (A..H and N..Q) to select the GPIO peripheral for STM32N6xx family
  * @param  GPIO_Init pointer to a GPIO_InitTypeDef structure that contains
  *         the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, const GPIO_InitTypeDef *GPIO_Init)
{
  uint32_t position = 0x00u;
  uint32_t iocurrent;
  uint32_t temp;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Init->Pin));
  assert_param(IS_GPIO_MODE(GPIO_Init->Mode));

  /* Configure the port pins */
  while (((GPIO_Init->Pin) >> position) != 0x00u)
  {
    /* Get current io position */
    iocurrent = (GPIO_Init->Pin) & (1uL << position);

    if (iocurrent != 0x00u)
    {
      /*--------------------- GPIO Mode Configuration ------------------------*/
      /* In case of Output or Alternate function mode selection */
      if (((GPIO_Init->Mode & GPIO_MODE) == MODE_OUTPUT) || ((GPIO_Init->Mode & GPIO_MODE) == MODE_AF))
      {
        /* Check the Speed parameter */
        assert_param(IS_GPIO_SPEED(GPIO_Init->Speed));

        /* Configure the IO Speed */
        temp = GPIOx->OSPEEDR;
        temp &= ~(GPIO_OSPEEDR_OSPEED0 << (position * GPIO_OSPEEDR_OSPEED1_Pos));
        temp |= (GPIO_Init->Speed << (position * GPIO_OSPEEDR_OSPEED1_Pos));
        GPIOx->OSPEEDR = temp;

        /* Configure the IO Output Type */
        temp = GPIOx->OTYPER;
        temp &= ~(GPIO_OTYPER_OT0 << position) ;
        temp |= (((GPIO_Init->Mode & OUTPUT_TYPE) >> OUTPUT_TYPE_Pos) << position);
        GPIOx->OTYPER = temp;
      }

      if (((GPIO_Init->Mode & GPIO_MODE) != MODE_ANALOG) ||
          (((GPIO_Init->Mode & GPIO_MODE) == MODE_ANALOG) && (GPIO_Init->Pull != GPIO_PULLUP)))
      {
        /* Check the Pull parameter */
        assert_param(IS_GPIO_PULL(GPIO_Init->Pull));

        /* Activate the Pull-up or Pull down resistor for the current IO */
        temp = GPIOx->PUPDR;
        temp &= ~(GPIO_PUPDR_PUPD0 << (position * GPIO_PUPDR_PUPD1_Pos));
        temp |= ((GPIO_Init->Pull) << (position * GPIO_PUPDR_PUPD1_Pos));
        GPIOx->PUPDR = temp;
      }

      /* In case of Alternate function mode selection */
      if ((GPIO_Init->Mode & GPIO_MODE) == MODE_AF)
      {
        /* Check the Alternate function parameters */
        assert_param(IS_GPIO_AF_INSTANCE(GPIOx));
        assert_param(IS_GPIO_AF(GPIO_Init->Alternate));

        /* Configure Alternate function mapped with the current IO */
        temp = GPIOx->AFR[position >> 3u];
        temp &= ~(0xFu << ((position & 0x07u) * GPIO_AFRL_AFSEL1_Pos));
        temp |= ((GPIO_Init->Alternate) << ((position & 0x07u) * GPIO_AFRL_AFSEL1_Pos));
        GPIOx->AFR[position >> 3u] = temp;
      }

      /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
      temp = GPIOx->MODER;
      temp &= ~(GPIO_MODER_MODE0 << (position * GPIO_MODER_MODE1_Pos));
      temp |= ((GPIO_Init->Mode & GPIO_MODE) << (position * GPIO_MODER_MODE1_Pos));
      GPIOx->MODER = temp;

      /*--------------------- EXTI Mode Configuration ------------------------*/
      /* Configure the External Interrupt or event for the current IO */
      if ((GPIO_Init->Mode & EXTI_MODE) != 0x00u)
      {
        temp = EXTI->EXTICR[position >> 2u];
        temp &= ~(0x0FuL << ((position & 0x03u) * EXTI_EXTICR1_EXTI1_Pos));
        temp |= (GPIO_GET_INDEX(GPIOx) << ((position & 0x03u) * EXTI_EXTICR1_EXTI1_Pos));
        EXTI->EXTICR[position >> 2u] = temp;

        /* Clear EXTI line configuration */
        temp = EXTI->IMR1;
        temp &= ~(iocurrent);
        if ((GPIO_Init->Mode & EXTI_IT) != 0x00u)
        {
          temp |= iocurrent;
        }
        EXTI->IMR1 = temp;

        temp = EXTI->EMR1;
        temp &= ~(iocurrent);
        if ((GPIO_Init->Mode & EXTI_EVT) != 0x00u)
        {
          temp |= iocurrent;
        }
        EXTI->EMR1 = temp;

        /* Clear Rising Falling edge configuration */
        temp = EXTI->RTSR1;
        temp &= ~(iocurrent);
        if ((GPIO_Init->Mode & TRIGGER_RISING) != 0x00u)
        {
          temp |= iocurrent;
        }
        EXTI->RTSR1 = temp;

        temp = EXTI->FTSR1;
        temp &= ~(iocurrent);
        if ((GPIO_Init->Mode & TRIGGER_FALLING) != 0x00u)
        {
          temp |= iocurrent;
        }
        EXTI->FTSR1 = temp;
      }
    }

    position++;
  }
}

/**
  * @brief  De-initialize the GPIOx peripheral registers to their default reset values.
  * @param  GPIOx where x can be (A..H and N..Q) to select the GPIO peripheral for STM32N6xx family
  * @param  GPIO_Pin specifies the port bit to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin)
{
  uint32_t position = 0x00u;
  uint32_t iocurrent;
  uint32_t tmp;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  /* Configure the port pins */
  while ((GPIO_Pin >> position) != 0x00u)
  {
    /* Get current io position */
    iocurrent = (GPIO_Pin) & (1uL << position);

    if (iocurrent != 0x00u)
    {
      /*------------------------- EXTI Mode Configuration --------------------*/
      /* Clear the External Interrupt or Event for the current IO */

      tmp = EXTI->EXTICR[position >> 2u];
      tmp &= (0x0FuL << ((position & 0x03u) * EXTI_EXTICR1_EXTI1_Pos));
      if (tmp == (GPIO_GET_INDEX(GPIOx) << ((position & 0x03u) * EXTI_EXTICR1_EXTI1_Pos)))
      {
        /* Clear EXTI line configuration */
        EXTI->IMR1 &= ~(iocurrent);
        EXTI->EMR1 &= ~(iocurrent);

        /* Clear Rising Falling edge configuration */
        EXTI->RTSR1 &= ~(iocurrent);
        EXTI->FTSR1 &= ~(iocurrent);

        tmp = 0x0FuL << ((position & 0x03u) * EXTI_EXTICR1_EXTI1_Pos);
        EXTI->EXTICR[position >> 2u] &= ~tmp;
      }

      /*------------------------- GPIO Mode Configuration --------------------*/
      /* Configure IO in Analog Mode */
      GPIOx->MODER |= (GPIO_MODER_MODE0 << (position * GPIO_MODER_MODE1_Pos));

      /* Configure the default Alternate Function in current IO */
      GPIOx->AFR[position >> 3u] &= ~(0xFu << ((position & 0x07u) * GPIO_AFRL_AFSEL1_Pos)) ;

      /* Configure the default value for IO Speed */
      GPIOx->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0 << (position * GPIO_OSPEEDR_OSPEED1_Pos));

      /* Configure the default value IO Output Type */
      GPIOx->OTYPER  &= ~(GPIO_OTYPER_OT0 << position) ;

      /* Deactivate the Pull-up and Pull-down resistor for the current IO */
      GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPD0 << (position * GPIO_PUPDR_PUPD1_Pos));

      /* Reset delay settings for the current IO */
      GPIOx->DELAYR[position >> 3u] &= ~(0xFu << ((position & 0x07u) * GPIO_DELAYRL_DLY1_Pos)) ;

      /* Reset control settings for the current IO */
      GPIOx->ADVCFGR[position >> 3u] &= ~(0xFu << ((position & 0x07u) * GPIO_ADVCFGRL_1_Pos)) ;
    }

    position++;
  }
}

/**
  * @}
  */

/** @addtogroup GPIO_Exported_Functions_Group2
  *  @brief GPIO Read, Write, Toggle, Lock and EXTI management functions.
  *
@verbatim
 ===============================================================================
                       ##### IO operation functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Read the specified input port pin.
  * @param  GPIOx where x can be (A..H and N..Q) to select the GPIO peripheral for STM32N6xx family
  * @param  GPIO_Pin specifies the port bit to read.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval The input port pin value.
  */
GPIO_PinState HAL_GPIO_ReadPin(const GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  GPIO_PinState bitstatus;

  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  if ((GPIOx->IDR & GPIO_Pin) != 0x00u)
  {
    bitstatus = GPIO_PIN_SET;
  }
  else
  {
    bitstatus = GPIO_PIN_RESET;
  }
  return bitstatus;
}

/**
  * @brief  Set or clear the selected data port bit.
  * @note   This function uses GPIOx_BSRR and GPIOx_BRR registers to allow atomic read/modify
  *         accesses. In this way, there is no risk of an IRQ occurring between
  *         the read and the modify access.
  * @param  GPIOx where x can be (A..H and N..Q) to select the GPIO peripheral for STM32N6xx family
  * @param  GPIO_Pin specifies the port bit to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @param  PinState specifies the value to be written to the selected bit.
  *         This parameter can be one of the GPIO_PinState enum values:
  *            @arg GPIO_PIN_RESET: to clear the port pin
  *            @arg GPIO_PIN_SET: to set the port pin
  * @retval None
  */
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_GPIO_PIN_ACTION(PinState));

  if (PinState != GPIO_PIN_RESET)
  {
    GPIOx->BSRR = (uint32_t)GPIO_Pin;
  }
  else
  {
    GPIOx->BRR = (uint32_t)GPIO_Pin;
  }
}

/**
  * @brief  Toggle the specified GPIO pin.
  * @param  GPIOx where x can be (A..H and N..Q) to select the GPIO peripheral for STM32N6xx family
  * @param  GPIO_Pin specifies the pin to be toggled.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  uint32_t odr;

  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  /* get current Output Data Register value */
  odr = GPIOx->ODR;

  /* Set selected pins that were at low level, and reset ones that were high */
  GPIOx->BSRR = ((odr & GPIO_Pin) << GPIO_NUMBER) | (~odr & GPIO_Pin);
}

/**
  * @brief  Set and clear several pins of a dedicated port in same cycle.
  * @param  GPIOx where x can be (A..H and N..Q) to select the GPIO peripheral for STM32N6xx family
  * @param  PinReset specifies the port bits to be reset
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15) or zero.
  * @param  PinSet specifies the port bits to be set
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15) or zero.
  * @note   Both PinReset and PinSet combinations shall not get any common bit, else
  *         assert would be triggered.
  * @note   At least one of the two parameters used to set or reset shall be different from zero.
  * @retval None
  */
void HAL_GPIO_WriteMultipleStatePin(GPIO_TypeDef *GPIOx, uint16_t PinReset, uint16_t PinSet)
{
  uint32_t tmp;

  /* Check the parameters */
  /* Make sure at least one parameter is different from zero and that there is no common pin */
  assert_param(IS_GPIO_PIN((uint32_t)PinReset | (uint32_t)PinSet));
  assert_param(IS_GPIO_COMMON_PIN(PinReset, PinSet));

  tmp = (((uint32_t)PinReset << 16) | PinSet);
  GPIOx->BSRR = tmp;
}

/**
  * @brief  Lock GPIO Pins configuration registers.
  * @note   The locked registers are GPIOx_MODER, GPIOx_OTYPER, GPIOx_OSPEEDR,
  *         GPIOx_PUPDR, GPIOx_AFRL, GPIOx_AFRH, GPIOx_DELAYRL, GPIOx_DELAYRH, GPIOx_ADVCFGRL, GPIOx_ADVCFGRH.
  * @note   The configuration of the locked GPIO pins can no longer be modified until the next reset.
  * @param  GPIOx where x can be (A..H and N..Q) to select the GPIO peripheral for STM32N6xx family
  * @param  GPIO_Pin specifies the port bits to be locked.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval HAL_OK if success, HAL_ERROR otherwise
  */
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  __IO uint32_t tmp = GPIO_LCKR_LCKK;

  /* Check the parameters */
  assert_param(IS_GPIO_LOCK_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  /* Apply lock key write sequence */
  tmp |= GPIO_Pin;
  /* Set LCKx bit(s): LCKK='1' + LCK[15-0] */
  GPIOx->LCKR = tmp;
  /* Reset LCKx bit(s): LCKK='0' + LCK[15-0] */
  GPIOx->LCKR = GPIO_Pin;
  /* Set LCKx bit(s): LCKK='1' + LCK[15-0] */
  GPIOx->LCKR = tmp;
  /* Read LCKK register. This read is mandatory to complete key lock sequence */
  tmp = GPIOx->LCKR;

  /* read again in order to confirm lock is active */
  if ((GPIOx->LCKR & GPIO_LCKR_LCKK) != 0x00u)
  {
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }
}

/**
  * @brief  Configure GPIO retime on specified GPIO pin.
  * @param  GPIOx where x can be (A..H and N..Q)
  * @param  GPIO_Pin specifies the port bit to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @param  pRet_Init pointer to a GPIO_RetimeTypeDef structure that contains
  *         the retime configuration information for the specified GPIO peripheral.
  * @retval None
  */
void HAL_GPIO_SetRetime(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, const GPIO_RetimeTypeDef *pRet_Init)
{
  uint32_t position = 0x00u;
  uint32_t iocurrent;
  uint32_t temp;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_GPIO_RETIME(pRet_Init->Retime));
  assert_param(IS_GPIO_CLOCK(pRet_Init->Edge));

  /* Configure the port pins */
  while ((GPIO_Pin >> position) != 0x00u)
  {
    /* Get current io position */
    iocurrent = (GPIO_Pin) & (1uL << position);

    if (iocurrent != 0x00u)
    {
      /* Configure the IO Retime */
      temp = GPIOx->ADVCFGR[position >> 3u];
      temp &= ~((GPIO_ADVCFGRL_RET0 | GPIO_ADVCFGRL_INVCLK0 | GPIO_ADVCFGRL_DE0)
                << ((position & 0x07u) * GPIO_ADVCFGRL_1_Pos));
      temp |= (pRet_Init->Retime << ((position & 0x07u) * GPIO_ADVCFGRL_1_Pos));
      temp |= (pRet_Init->Edge << ((position & 0x07u) * GPIO_ADVCFGRL_1_Pos));
      GPIOx->ADVCFGR[position >> 3u] = temp;
    }
    position++;
  }
}

/**
  * @brief  Get GPIO retime configuration on specified GPIO pin.
  * @param  GPIOx where x can be (A..H and N..Q)
  * @param  GPIO_Pin specifies the port bit to read.
  *         This parameter can be GPIO_PIN_x where x can be (0..15).
  * @param  pRet_Init pointer to the return GPIO_RetimeTypeDef structure that contains the retime information
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_GPIO_GetRetime(const GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_RetimeTypeDef *pRet_Init)
{
  uint32_t position;
  uint32_t index;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_SINGLE_PIN(GPIO_Pin));

  /* Check null pointer */
  if (pRet_Init == NULL)
  {
    return HAL_ERROR;
  }

  /* get IO position */
  position = POSITION_VAL(GPIO_Pin);

  /* Get the IO advanced configuration */
  index = (position & 0x07u) * GPIO_ADVCFGRL_1_Pos;
  pRet_Init->Edge = ((GPIOx->ADVCFGR[position >> 3u]
                      & ((GPIO_ADVCFGRL_DE0 << index) | (GPIO_ADVCFGRL_INVCLK0 << index))) >> index);
  pRet_Init->Retime = ((GPIOx->ADVCFGR[position >> 3u] & (GPIO_ADVCFGRL_RET0 << index)) >> index);

  return HAL_OK;
}


/**
  * @brief  Configure GPIO delay on specified GPIO pin.
  * @param  GPIOx where x can be (A..H and N..Q)
  * @param  GPIO_Pin specifies the port bit to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @param  pDly_Init pointer to a GPIO_DelayTypeDef structure that contains
  *         the delay configuration information for the specified GPIO peripheral.
  * @retval None
  */
void HAL_GPIO_SetDelay(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, const GPIO_DelayTypeDef *pDly_Init)
{
  uint32_t position = 0x00u;
  uint32_t iocurrent;
  uint32_t temp;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_GPIO_DELAY(pDly_Init->Delay));
  assert_param(IS_GPIO_PATH(pDly_Init->Path));

  /* Configure the port pins */
  while ((GPIO_Pin >> position) != 0x00u)
  {
    /* Get current io position */
    iocurrent = (GPIO_Pin) & (1uL << position);

    if (iocurrent != 0x00u)
    {
      /* Configure the IO Delay path */
      temp = GPIOx->ADVCFGR[position >> 3u];
      temp &= ~(GPIO_ADVCFGRL_DLYPATH0 << ((position & 0x07u) * GPIO_ADVCFGRL_1_Pos));
      temp |= (pDly_Init->Path << ((position & 0x07u) * GPIO_ADVCFGRL_1_Pos));
      GPIOx->ADVCFGR[position >> 3u] = temp;

      /* Configure the IO Delay */
      temp = GPIOx->DELAYR[position >> 3u];
      temp &= ~(GPIO_DELAYRL_DLY0_Msk << ((position & 0x07u) * GPIO_ADVCFGRL_1_Pos));
      temp |= (pDly_Init->Delay << ((position & 0x07u) * GPIO_ADVCFGRL_1_Pos));
      GPIOx->DELAYR[position >> 3u] = temp;
    }
    position++;
  }
}

/**
  * @brief  Get GPIO delay configuration on specified GPIO pin.
  * @param  GPIOx where x can be (A..H and N..Q)
  * @param  GPIO_Pin specifies the port bit to read.
  *         This parameter can be GPIO_PIN_x where x can be (0..15).
  * @param  pDly_Init pointer to the return GPIO_DelayTypeDef structure that contains the delay information
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_GPIO_GetDelay(const GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_DelayTypeDef *pDly_Init)
{
  uint32_t position;
  uint32_t index;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_SINGLE_PIN(GPIO_Pin));

  /* Check null pointer */
  if (pDly_Init == NULL)
  {
    return HAL_ERROR;
  }

  /* get IO position */
  position = POSITION_VAL(GPIO_Pin);

  /* Get the IO advanced configuration */
  index = (position & 0x07u) * GPIO_ADVCFGRL_1_Pos;
  pDly_Init->Delay = ((GPIOx->DELAYR[position >> 3u] & (GPIO_DELAYRL_DLY0_Msk << index)) >> index);
  pDly_Init->Path = ((GPIOx->ADVCFGR[position >> 3u] & (GPIO_ADVCFGRL_DLYPATH0 << index)) >> index);

  return HAL_OK;
}

/**
  * @brief  Handle EXTI interrupt request.
  * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)
{
  /* EXTI line interrupt detected */
  if (__HAL_GPIO_EXTI_GET_RISING_IT(GPIO_Pin) != 0x00u)
  {
    __HAL_GPIO_EXTI_CLEAR_RISING_IT(GPIO_Pin);
    HAL_GPIO_EXTI_Rising_Callback(GPIO_Pin);
  }

  if (__HAL_GPIO_EXTI_GET_FALLING_IT(GPIO_Pin) != 0x00u)
  {
    __HAL_GPIO_EXTI_CLEAR_FALLING_IT(GPIO_Pin);
    HAL_GPIO_EXTI_Falling_Callback(GPIO_Pin);
  }
}

/**
  * @brief  EXTI line rising detection callback.
  * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
__weak void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Rising_Callback could be implemented in the user file
   */
}

/**
  * @brief  EXTI line falling detection callback.
  * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
__weak void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Falling_Callback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup GPIO_Exported_Functions_Group3 IO attributes management functions
  *  @brief GPIO attributes management functions.
  *
@verbatim
 ===============================================================================
                       ##### IO attributes functions #####
 ===============================================================================

@endverbatim
  * @{
  */

#if defined (CPU_IN_SECURE_STATE)

/**
  * @brief  Lock security and privilege configuration of several pins for a dedicated port.
  * @note   When the lock sequence has been applied on a port bit, the value of this port bit can no longer
  *         be modified until the next reset. Set sequence only available in secure and privilege.
  * @note   Each lock bit freezes a security configuration register (control and alternate function registers).
  * @param  GPIOx GPIO Port
  * @param  GPIO_Pin specifies the port bit to be locked.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void HAL_GPIO_LockPinAttributes(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  /* Lock the pins */
  GPIOx->RCFGLOCKR = (uint32_t)GPIO_Pin;
}

/**
  * @brief  Get lock security and privilege configuration of several pins for a dedicated port.
  * @param  GPIOx GPIO Port
  * @retval Lock status for all pins. Bitx are set to 1 for pinx locked, otherwise 0.
  */
uint32_t HAL_GPIO_GetLockPinAttributes(const GPIO_TypeDef *GPIOx)
{

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));

  /* Lock the pins */
  return (GPIOx->RCFGLOCKR);

}

#endif /* CPU_IN_SECURE_STATE */

/**
  * @brief  Configure the GPIO pins attributes.
  * @note   Set a pin to secure is only available in secure and privilege
  *         Set a pin to privilege is only available in privilege
  * @param  GPIOx where x can be (A..H and N..Q) to select the GPIO peripheral for STM32N6xx family
  * @param  GPIO_Pin : GPIO_Pin specifies the pin(s) to configure the secure/privilege attribute
  * @param  PinAttributes: PinAttributes can be one or a combination of the following values :
  *            @arg @ref GPIO_PIN_PRIV         Privileged-only access
  *            @arg @ref GPIO_PIN_NPRIV        Privileged/Non-privileged access
  *            @arg @ref GPIO_PIN_SEC          Secure-only access
  *            @arg @ref GPIO_PIN_NSEC         Secure/Non-secure access
  * @retval None.
  */
void HAL_GPIO_ConfigPinAttributes(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t PinAttributes)
{
#if defined CPU_IN_SECURE_STATE
  uint32_t sec;
#endif /* CPU_IN_SECURE_STATE */
  uint32_t priv;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_GPIO_PIN_ATTRIBUTES(PinAttributes));

#if defined CPU_IN_SECURE_STATE
  /* Configure the port pins */
  sec = GPIOx->SECCFGR;
  if ((PinAttributes & GPIO_PIN_SEC) == GPIO_PIN_SEC)
  {
    sec |= (uint32_t)GPIO_Pin;
  }
  else if ((PinAttributes & GPIO_PIN_NSEC) == GPIO_PIN_NSEC)
  {
    sec &= ~((uint32_t)GPIO_Pin);
  }
  else
  {
    /* do nothing */
  }
  GPIOx->SECCFGR = sec;
#endif /* CPU_IN_SECURE_STATE */

  priv = GPIOx->PRIVCFGR;
  if ((PinAttributes & GPIO_PIN_PRIV) == GPIO_PIN_PRIV)
  {
    priv |= (uint32_t)GPIO_Pin;
  }
  else if ((PinAttributes & GPIO_PIN_NPRIV) == GPIO_PIN_NPRIV)
  {
    priv &= ~((uint32_t)GPIO_Pin);
  }
  else
  {
    /* do nothing */
  }

  GPIOx->PRIVCFGR = priv;
}

/**
  * @brief  Get the GPIO pins attributes.
  * @param  GPIOx where x can be (A..H and N..Q) to select the GPIO peripheral for STM32N6xx family
  * @param  GPIO_Pin specifies the port bit to read.
  *         This parameter can be GPIO_PIN_x where x can be (0..15).
  * @param  pPinAttributes: pPinAttributes pointer to return the pin attributes.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_GPIO_GetConfigPinAttributes(const GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                                                  uint32_t *pPinAttributes)
{

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_SINGLE_PIN(GPIO_Pin));

  /* Check null pointer */
  if (pPinAttributes == NULL)
  {
    return HAL_ERROR;
  }

  if ((GPIOx->SECCFGR & GPIO_Pin) != 0x00U)
  {
    *pPinAttributes = GPIO_PIN_SEC;
  }
  else
  {
    *pPinAttributes = GPIO_PIN_NSEC;
  }

  if ((GPIOx->PRIVCFGR & GPIO_Pin) != 0x00U)
  {
    *pPinAttributes |= GPIO_PIN_PRIV;
  }
  else
  {
    *pPinAttributes |= GPIO_PIN_NPRIV;
  }

  return HAL_OK;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_GPIO_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */
