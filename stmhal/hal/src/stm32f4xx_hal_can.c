/**
  ******************************************************************************
  * @file    stm32f4xx_hal_can.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Controller Area Network (CAN) peripheral:
  *           + Initialization and de-initialization functions 
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State and Error functions
  *
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
    [..]            
      (#) Enable the CAN controller interface clock using 
          __CAN1_CLK_ENABLE() for CAN1 and __CAN1_CLK_ENABLE() for CAN2
      -@- In case you are using CAN2 only, you have to enable the CAN1 clock.
       
      (#) CAN pins configuration
        (++) Enable the clock for the CAN GPIOs using the following function:
             __GPIOx_CLK_ENABLE()   
        (++) Connect and configure the involved CAN pins to AF9 using the 
              following function HAL_GPIO_Init() 
              
      (#) Initialise and configure the CAN using CAN_Init() function.   
                 
      (#) Transmit the desired CAN frame using HAL_CAN_Transmit() function.
           
      (#) Receive a CAN frame using HAL_CAN_Recieve() function.

     *** Polling mode IO operation ***
     =================================
     [..]    
       (+) Start the CAN peripheral transmission and wait the end of this operation 
           using HAL_CAN_Transmit(), at this stage user can specify the value of timeout
           according to his end application
       (+) Start the CAN peripheral reception and wait the end of this operation 
           using HAL_CAN_Receive(), at this stage user can specify the value of timeout
           according to his end application 
       
     *** Interrupt mode IO operation ***    
     ===================================
     [..]    
       (+) Start the CAN peripheral transmission using HAL_CAN_Transmit_IT()
       (+) Start the CAN peripheral reception using HAL_CAN_Receive_IT()         
       (+) Use HAL_CAN_IRQHandler() called under the used CAN Interrupt subroutine
       (+) At CAN end of transmission HAL_CAN_TxCpltCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_CAN_TxCpltCallback 
       (+) In case of CAN Error, HAL_CAN_ErrorCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_CAN_ErrorCallback
 
     *** CAN HAL driver macros list ***
     ============================================= 
     [..]
       Below the list of most used macros in CAN HAL driver.
       
      (+) __HAL_CAN_ENABLE_IT: Enable the specified CAN interrupts
      (+) __HAL_CAN_DISABLE_IT: Disable the specified CAN interrupts
      (+) __HAL_CAN_GET_IT_SOURCE: Check if the specified CAN interrupt source is enabled or disabled
      (+) __HAL_CAN_CLEAR_FLAG: Clear the CAN's pending flags
      (+) __HAL_CAN_GET_FLAG: Get the selected CAN's flag status
      
     [..] 
      (@) You can refer to the CAN HAL driver header file for more useful macros 
                
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

/** @defgroup CAN 
  * @brief CAN driver modules
  * @{
  */ 
  
#ifdef HAL_CAN_MODULE_ENABLED  

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef CAN_Receive_IT(CAN_HandleTypeDef* hcan, uint8_t FIFONumber);
static HAL_StatusTypeDef CAN_Transmit_IT(CAN_HandleTypeDef* hcan);
/* Private functions ---------------------------------------------------------*/

/** @defgroup CAN_Private_Functions
  * @{
  */

/** @defgroup CAN_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
  ==============================================================================
              ##### Initialization and de-initialization functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the CAN. 
      (+) De-initialize the CAN. 
         
@endverbatim
  * @{
  */
  
/**
  * @brief  Initializes the CAN peripheral according to the specified
  *         parameters in the CAN_InitStruct.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_Init(CAN_HandleTypeDef* hcan)
{
  uint32_t InitStatus = 3;
  uint32_t timeout = 0;
  
  /* Check CAN handle */
  if(hcan == NULL)
  {
     return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_CAN_ALL_INSTANCE(hcan->Instance));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.TTCM));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.ABOM));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.AWUM));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.NART));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.RFLM));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.TXFP));
  assert_param(IS_CAN_MODE(hcan->Init.Mode));
  assert_param(IS_CAN_SJW(hcan->Init.SJW));
  assert_param(IS_CAN_BS1(hcan->Init.BS1));
  assert_param(IS_CAN_BS2(hcan->Init.BS2));
  assert_param(IS_CAN_PRESCALER(hcan->Init.Prescaler));
  

  if(hcan->State == HAL_CAN_STATE_RESET)
  {    
    /* Init the low level hardware */
    HAL_CAN_MspInit(hcan);
  }
  
  /* Initialize the CAN state*/
  hcan->State = HAL_CAN_STATE_BUSY;
  
  /* Exit from sleep mode */
  hcan->Instance->MCR &= (~(uint32_t)CAN_MCR_SLEEP);

  /* Request initialisation */
  hcan->Instance->MCR |= CAN_MCR_INRQ ;
  
  /* Get timeout */
  timeout = HAL_GetTick() + 10;   
  
  /* Wait the acknowledge */
  while((hcan->Instance->MSR & CAN_MSR_INAK) != CAN_MSR_INAK)
  {
    if(HAL_GetTick() >= timeout)
    {
      hcan->State= HAL_CAN_STATE_TIMEOUT;
      /* Process unlocked */
      __HAL_UNLOCK(hcan);
      return HAL_TIMEOUT;
    }
  }

  /* Check acknowledge */
  if ((hcan->Instance->MSR & CAN_MSR_INAK) != CAN_MSR_INAK)
  {
    InitStatus = CAN_INITSTATUS_FAILED;
  }
  else 
  {
    /* Set the time triggered communication mode */
    if (hcan->Init.TTCM == ENABLE)
    {
      hcan->Instance->MCR |= CAN_MCR_TTCM;
    }
    else
    {
      hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_TTCM;
    }

    /* Set the automatic bus-off management */
    if (hcan->Init.ABOM == ENABLE)
    {
      hcan->Instance->MCR |= CAN_MCR_ABOM;
    }
    else
    {
      hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_ABOM;
    }

    /* Set the automatic wake-up mode */
    if (hcan->Init.AWUM == ENABLE)
    {
      hcan->Instance->MCR |= CAN_MCR_AWUM;
    }
    else
    {
      hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_AWUM;
    }

    /* Set the no automatic retransmission */
    if (hcan->Init.NART == ENABLE)
    {
      hcan->Instance->MCR |= CAN_MCR_NART;
    }
    else
    {
      hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_NART;
    }

    /* Set the receive FIFO locked mode */
    if (hcan->Init.RFLM == ENABLE)
    {
      hcan->Instance->MCR |= CAN_MCR_RFLM;
    }
    else
    {
      hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_RFLM;
    }

    /* Set the transmit FIFO priority */
    if (hcan->Init.TXFP == ENABLE)
    {
      hcan->Instance->MCR |= CAN_MCR_TXFP;
    }
    else
    {
      hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_TXFP;
    }

    /* Set the bit timing register */
    hcan->Instance->BTR = (uint32_t)((uint32_t)hcan->Init.Mode) | \
                ((uint32_t)hcan->Init.SJW) | \
                ((uint32_t)hcan->Init.BS1) | \
                ((uint32_t)hcan->Init.BS2) | \
               ((uint32_t)hcan->Init.Prescaler - 1);

    /* Request leave initialisation */
    hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_INRQ;

    /* Get timeout */
    timeout = HAL_GetTick() + 10;   
   
   /* Wait the acknowledge */
   while((hcan->Instance->MSR & CAN_MSR_INAK) == CAN_MSR_INAK)
   {
     if(HAL_GetTick() >= timeout)
     {
       hcan->State= HAL_CAN_STATE_TIMEOUT;
       /* Process unlocked */
       __HAL_UNLOCK(hcan);
       return HAL_TIMEOUT;
     }
   }

    /* Check acknowledged */
    if ((hcan->Instance->MSR & CAN_MSR_INAK) == CAN_MSR_INAK)
    {
      InitStatus = CAN_INITSTATUS_FAILED;
    }
    else
    {
      InitStatus = CAN_INITSTATUS_SUCCESS;
    }
  }
 
  if(InitStatus == CAN_INITSTATUS_SUCCESS)
  {
    /* Set CAN error code to none */
    hcan->ErrorCode = HAL_CAN_ERROR_NONE;
    
    /* Initialize the CAN state */
    hcan->State = HAL_CAN_STATE_READY;
  
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Initialize the CAN state */
    hcan->State = HAL_CAN_STATE_ERROR;
    
    /* Return function status */
    return HAL_ERROR;
  }
}

/**
  * @brief  Configures the CAN reception filter according to the specified
  *         parameters in the CAN_FilterInitStruct.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  sFilterConfig: pointer to a CAN_FilterConfTypeDef structure that
  *         contains the filter configuration information.
  * @retval None
  */
HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef* hcan, CAN_FilterConfTypeDef* sFilterConfig)
{
  uint32_t filternbrbitpos = 0;
  
  /* Check the parameters */
  assert_param(IS_CAN_FILTER_NUMBER(sFilterConfig->FilterNumber));
  assert_param(IS_CAN_FILTER_MODE(sFilterConfig->FilterMode));
  assert_param(IS_CAN_FILTER_SCALE(sFilterConfig->FilterScale));
  assert_param(IS_CAN_FILTER_FIFO(sFilterConfig->FilterFIFOAssignment));
  assert_param(IS_FUNCTIONAL_STATE(sFilterConfig->FilterActivation));
  assert_param(IS_CAN_BANKNUMBER(sFilterConfig->BankNumber));
  
  filternbrbitpos = ((uint32_t)1) << sFilterConfig->FilterNumber;

  /* Initialisation mode for the filter */
  CAN1->FMR |= (uint32_t)CAN_FMR_FINIT;
  
  /* Select the start slave bank */
  CAN1->FMR &= ~((uint32_t)CAN_FMR_CAN2SB);
  CAN1->FMR |= (uint32_t)(sFilterConfig->BankNumber << 8);
     
  /* Filter Deactivation */
  CAN1->FA1R &= ~(uint32_t)filternbrbitpos;

  /* Filter Scale */
  if (sFilterConfig->FilterScale == CAN_FILTERSCALE_16BIT)
  {
    /* 16-bit scale for the filter */
    CAN1->FS1R &= ~(uint32_t)filternbrbitpos;

    /* First 16-bit identifier and First 16-bit mask */
    /* Or First 16-bit identifier and Second 16-bit identifier */
    CAN1->sFilterRegister[sFilterConfig->FilterNumber].FR1 = 
       ((0x0000FFFF & (uint32_t)sFilterConfig->FilterMaskIdLow) << 16) |
        (0x0000FFFF & (uint32_t)sFilterConfig->FilterIdLow);

    /* Second 16-bit identifier and Second 16-bit mask */
    /* Or Third 16-bit identifier and Fourth 16-bit identifier */
    CAN1->sFilterRegister[sFilterConfig->FilterNumber].FR2 = 
       ((0x0000FFFF & (uint32_t)sFilterConfig->FilterMaskIdHigh) << 16) |
        (0x0000FFFF & (uint32_t)sFilterConfig->FilterIdHigh);
  }

  if (sFilterConfig->FilterScale == CAN_FILTERSCALE_32BIT)
  {
    /* 32-bit scale for the filter */
    CAN1->FS1R |= filternbrbitpos;
    /* 32-bit identifier or First 32-bit identifier */
    CAN1->sFilterRegister[sFilterConfig->FilterNumber].FR1 = 
       ((0x0000FFFF & (uint32_t)sFilterConfig->FilterIdHigh) << 16) |
        (0x0000FFFF & (uint32_t)sFilterConfig->FilterIdLow);
    /* 32-bit mask or Second 32-bit identifier */
    CAN1->sFilterRegister[sFilterConfig->FilterNumber].FR2 = 
       ((0x0000FFFF & (uint32_t)sFilterConfig->FilterMaskIdHigh) << 16) |
        (0x0000FFFF & (uint32_t)sFilterConfig->FilterMaskIdLow);
  }

  /* Filter Mode */
  if (sFilterConfig->FilterMode == CAN_FILTERMODE_IDMASK)
  {
    /*Id/Mask mode for the filter*/
    CAN1->FM1R &= ~(uint32_t)filternbrbitpos;
  }
  else /* CAN_FilterInitStruct->CAN_FilterMode == CAN_FilterMode_IdList */
  {
    /*Identifier list mode for the filter*/
    CAN1->FM1R |= (uint32_t)filternbrbitpos;
  }

  /* Filter FIFO assignment */
  if (sFilterConfig->FilterFIFOAssignment == CAN_FILTER_FIFO0)
  {
    /* FIFO 0 assignation for the filter */
    CAN1->FFA1R &= ~(uint32_t)filternbrbitpos;
  }

  if (sFilterConfig->FilterFIFOAssignment == CAN_FILTER_FIFO1)
  {
    /* FIFO 1 assignation for the filter */
    CAN1->FFA1R |= (uint32_t)filternbrbitpos;
  }
  
  /* Filter activation */
  if (sFilterConfig->FilterActivation == ENABLE)
  {
    CAN1->FA1R |= filternbrbitpos;
  }

  /* Leave the initialisation mode for the filter */
  CAN1->FMR &= ~((uint32_t)CAN_FMR_FINIT);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Deinitializes the CANx peripheral registers to their default reset values. 
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_DeInit(CAN_HandleTypeDef* hcan)
{
  /* Check CAN handle */
  if(hcan == NULL)
  {
     return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_CAN_ALL_INSTANCE(hcan->Instance));
  
  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_BUSY;
  
  /* DeInit the low level hardware */
  HAL_CAN_MspDeInit(hcan);
  
  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hcan);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CAN MSP.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval None
  */
__weak void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief  DeInitializes the CAN MSP.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval None
  */
__weak void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_MspDeInit could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup CAN_Group2 IO operation functions
 *  @brief    IO operation functions 
 *
@verbatim   
  ==============================================================================
                      ##### IO operation functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) Transmit a CAN frame message.
      (+) Receive a CAN frame message.
      (+) Enter CAN peripheral in sleep mode. 
      (+) Wake up the CAN peripheral from sleep mode.
               
@endverbatim
  * @{
  */

/**
  * @brief  Initiates and transmits a CAN frame message.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @param  Timeout: Specify Timeout value   
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_Transmit(CAN_HandleTypeDef* hcan, uint32_t Timeout)
{
  uint32_t  transmitmailbox = 5;
  
  uint32_t timeout;
   
  /* Check the parameters */
  assert_param(IS_CAN_IDTYPE(hcan->pTxMsg->IDE));
  assert_param(IS_CAN_RTR(hcan->pTxMsg->RTR));
  assert_param(IS_CAN_DLC(hcan->pTxMsg->DLC));
  
  /* Process locked */
  __HAL_LOCK(hcan);
  
  if(hcan->State == HAL_CAN_STATE_BUSY_RX) 
  {
    /* Change CAN state */
    hcan->State = HAL_CAN_STATE_BUSY_TX_RX;
  }
  else
  {
    /* Change CAN state */
    hcan->State = HAL_CAN_STATE_BUSY_TX;
  }
  
  /* Select one empty transmit mailbox */
  if ((hcan->Instance->TSR&CAN_TSR_TME0) == CAN_TSR_TME0)
  {
    transmitmailbox = 0;
  }
  else if ((hcan->Instance->TSR&CAN_TSR_TME1) == CAN_TSR_TME1)
  {
    transmitmailbox = 1;
  }
  else if ((hcan->Instance->TSR&CAN_TSR_TME2) == CAN_TSR_TME2)
  {
    transmitmailbox = 2;
  }
  else
  {
    transmitmailbox = CAN_TXSTATUS_NOMAILBOX;
  }

  if (transmitmailbox != CAN_TXSTATUS_NOMAILBOX)
  {
    /* Set up the Id */
    hcan->Instance->sTxMailBox[transmitmailbox].TIR &= CAN_TI0R_TXRQ;
    if (hcan->pTxMsg->IDE == CAN_ID_STD)
    {
      assert_param(IS_CAN_STDID(hcan->pTxMsg->StdId));  
      hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->StdId << 21) | \
                                                  hcan->pTxMsg->RTR);
    }
    else
    {
      assert_param(IS_CAN_EXTID(hcan->pTxMsg->ExtId));
      hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->ExtId << 3) | \
                                                  hcan->pTxMsg->IDE | \
                                                  hcan->pTxMsg->RTR);
    }
    
    /* Set up the DLC */
    hcan->pTxMsg->DLC &= (uint8_t)0x0000000F;
    hcan->Instance->sTxMailBox[transmitmailbox].TDTR &= (uint32_t)0xFFFFFFF0;
    hcan->Instance->sTxMailBox[transmitmailbox].TDTR |= hcan->pTxMsg->DLC;

    /* Set up the data field */
    hcan->Instance->sTxMailBox[transmitmailbox].TDLR = (((uint32_t)hcan->pTxMsg->Data[3] << 24) | 
                                             ((uint32_t)hcan->pTxMsg->Data[2] << 16) |
                                             ((uint32_t)hcan->pTxMsg->Data[1] << 8) | 
                                             ((uint32_t)hcan->pTxMsg->Data[0]));
    hcan->Instance->sTxMailBox[transmitmailbox].TDHR = (((uint32_t)hcan->pTxMsg->Data[7] << 24) | 
                                             ((uint32_t)hcan->pTxMsg->Data[6] << 16) |
                                             ((uint32_t)hcan->pTxMsg->Data[5] << 8) |
                                             ((uint32_t)hcan->pTxMsg->Data[4]));
    /* Request transmission */
    hcan->Instance->sTxMailBox[transmitmailbox].TIR |= CAN_TI0R_TXRQ;
  
    /* Get timeout */
    timeout = HAL_GetTick() + Timeout;   
  
    /* Check End of transmission flag */
    while(!(__HAL_CAN_TRANSMIT_STATUS(hcan, transmitmailbox)))
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if(HAL_GetTick() >= timeout)
        {
          hcan->State = HAL_CAN_STATE_TIMEOUT;
          /* Process unlocked */
          __HAL_UNLOCK(hcan);
          return HAL_TIMEOUT;
        }
      }
    }
    if(hcan->State == HAL_CAN_STATE_BUSY_TX_RX) 
    {
      /* Change CAN state */
      hcan->State = HAL_CAN_STATE_BUSY_RX;
      
      /* Process unlocked */
      __HAL_UNLOCK(hcan);
    }
    else
    {
      /* Change CAN state */
      hcan->State = HAL_CAN_STATE_READY;
      
      /* Process unlocked */
      __HAL_UNLOCK(hcan);
    }
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Change CAN state */
    hcan->State = HAL_CAN_STATE_ERROR; 
    
    /* Return function status */
    return HAL_ERROR;
  }
}

/**
  * @brief  Initiates and transmits a CAN frame message.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_Transmit_IT(CAN_HandleTypeDef* hcan)
{
  uint32_t  transmitmailbox = 5;
  uint32_t tmp = 0;
  
  /* Check the parameters */
  assert_param(IS_CAN_IDTYPE(hcan->pTxMsg->IDE));
  assert_param(IS_CAN_RTR(hcan->pTxMsg->RTR));
  assert_param(IS_CAN_DLC(hcan->pTxMsg->DLC));
  
  tmp = hcan->State;
  if((tmp == HAL_CAN_STATE_READY) || (tmp == HAL_CAN_STATE_BUSY_RX))
  {
    /* Process Locked */
    __HAL_LOCK(hcan);
    
    /* Select one empty transmit mailbox */
    if((hcan->Instance->TSR&CAN_TSR_TME0) == CAN_TSR_TME0)
    {
      transmitmailbox = 0;
    }
    else if((hcan->Instance->TSR&CAN_TSR_TME1) == CAN_TSR_TME1)
    {
      transmitmailbox = 1;
    }
    else if((hcan->Instance->TSR&CAN_TSR_TME2) == CAN_TSR_TME2)
    {
      transmitmailbox = 2;
    }
    else
    {
      transmitmailbox = CAN_TXSTATUS_NOMAILBOX;
    }

    if(transmitmailbox != CAN_TXSTATUS_NOMAILBOX)
    {
      /* Set up the Id */
      hcan->Instance->sTxMailBox[transmitmailbox].TIR &= CAN_TI0R_TXRQ;
      if(hcan->pTxMsg->IDE == CAN_ID_STD)
      {
        assert_param(IS_CAN_STDID(hcan->pTxMsg->StdId));  
        hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->StdId << 21) | \
                                                  hcan->pTxMsg->RTR);
      }
      else
      {
        assert_param(IS_CAN_EXTID(hcan->pTxMsg->ExtId));
        hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->ExtId << 3) | \
                                                  hcan->pTxMsg->IDE | \
                                                  hcan->pTxMsg->RTR);
      }
    
      /* Set up the DLC */
      hcan->pTxMsg->DLC &= (uint8_t)0x0000000F;
      hcan->Instance->sTxMailBox[transmitmailbox].TDTR &= (uint32_t)0xFFFFFFF0;
      hcan->Instance->sTxMailBox[transmitmailbox].TDTR |= hcan->pTxMsg->DLC;

      /* Set up the data field */
      hcan->Instance->sTxMailBox[transmitmailbox].TDLR = (((uint32_t)hcan->pTxMsg->Data[3] << 24) | 
                                             ((uint32_t)hcan->pTxMsg->Data[2] << 16) |
                                             ((uint32_t)hcan->pTxMsg->Data[1] << 8) | 
                                             ((uint32_t)hcan->pTxMsg->Data[0]));
      hcan->Instance->sTxMailBox[transmitmailbox].TDHR = (((uint32_t)hcan->pTxMsg->Data[7] << 24) | 
                                             ((uint32_t)hcan->pTxMsg->Data[6] << 16) |
                                             ((uint32_t)hcan->pTxMsg->Data[5] << 8) |
                                             ((uint32_t)hcan->pTxMsg->Data[4]));
    
      if(hcan->State == HAL_CAN_STATE_BUSY_RX) 
      {
        /* Change CAN state */
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX;
      }
      else
      {
        /* Change CAN state */
        hcan->State = HAL_CAN_STATE_BUSY_TX;
      }
      
      /* Set CAN error code to none */
      hcan->ErrorCode = HAL_CAN_ERROR_NONE;
      
      /* Process Unlocked */
      __HAL_UNLOCK(hcan);
      
      /* Enable Error warning Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_EWG);
      
      /* Enable Error passive Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_EPV);
      
      /* Enable Bus-off Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_BOF);
      
      /* Enable Last error code Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_LEC);
      
      /* Enable Error Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_ERR);
      
      /* Enable Transmit mailbox empty Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_TME);
      
      /* Request transmission */
      hcan->Instance->sTxMailBox[transmitmailbox].TIR |= CAN_TI0R_TXRQ;
    }
  }
  else
  {
    return HAL_BUSY;
  }
  
  return HAL_OK;
}

/**
  * @brief  Receives a correct CAN frame.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @param  FIFONumber: FIFO Number value                 
  * @param  Timeout: Specify Timeout value        
  * @retval HAL status
  * @retval None
  */
HAL_StatusTypeDef HAL_CAN_Receive(CAN_HandleTypeDef* hcan, uint8_t FIFONumber, uint32_t Timeout)
{
  uint32_t timeout;
   
  /* Check the parameters */
  assert_param(IS_CAN_FIFO(FIFONumber));
  
  /* Process locked */
  __HAL_LOCK(hcan);
  
  if(hcan->State == HAL_CAN_STATE_BUSY_TX) 
  {
    /* Change CAN state */
    hcan->State = HAL_CAN_STATE_BUSY_TX_RX;
  }
  else
  {
    /* Change CAN state */
    hcan->State = HAL_CAN_STATE_BUSY_RX;
  }
    
  /* Get timeout */
  timeout = HAL_GetTick() + Timeout;   
  
  /* Check pending message */
  while(__HAL_CAN_MSG_PENDING(hcan, FIFONumber) == 0)
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        hcan->State = HAL_CAN_STATE_TIMEOUT;
        /* Process unlocked */
        __HAL_UNLOCK(hcan);
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Get the Id */
  hcan->pRxMsg->IDE = (uint8_t)0x04 & hcan->Instance->sFIFOMailBox[FIFONumber].RIR;
  if (hcan->pRxMsg->IDE == CAN_ID_STD)
  {
    hcan->pRxMsg->StdId = (uint32_t)0x000007FF & (hcan->Instance->sFIFOMailBox[FIFONumber].RIR >> 21);
  }
  else
  {
    hcan->pRxMsg->ExtId = (uint32_t)0x1FFFFFFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RIR >> 3);
  }
  
  hcan->pRxMsg->RTR = (uint8_t)0x02 & hcan->Instance->sFIFOMailBox[FIFONumber].RIR;
  /* Get the DLC */
  hcan->pRxMsg->DLC = (uint8_t)0x0F & hcan->Instance->sFIFOMailBox[FIFONumber].RDTR;
  /* Get the FMI */
  hcan->pRxMsg->FMI = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDTR >> 8);
  /* Get the data field */
  hcan->pRxMsg->Data[0] = (uint8_t)0xFF & hcan->Instance->sFIFOMailBox[FIFONumber].RDLR;
  hcan->pRxMsg->Data[1] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 8);
  hcan->pRxMsg->Data[2] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 16);
  hcan->pRxMsg->Data[3] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 24);
  hcan->pRxMsg->Data[4] = (uint8_t)0xFF & hcan->Instance->sFIFOMailBox[FIFONumber].RDHR;
  hcan->pRxMsg->Data[5] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 8);
  hcan->pRxMsg->Data[6] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 16);
  hcan->pRxMsg->Data[7] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 24);
  
  /* Release the FIFO */
  if(FIFONumber == CAN_FIFO0)
  {
    /* Release FIFO0 */
    __HAL_CAN_FIFO_RELEASE(hcan, CAN_FIFO0);
  }
  else /* FIFONumber == CAN_FIFO1 */
  {
    /* Release FIFO1 */
    __HAL_CAN_FIFO_RELEASE(hcan, CAN_FIFO1);
  }
  
  if(hcan->State == HAL_CAN_STATE_BUSY_TX_RX) 
  {
    /* Change CAN state */
    hcan->State = HAL_CAN_STATE_BUSY_TX;
    
    /* Process unlocked */
    __HAL_UNLOCK(hcan);
  }
  else
  {
    /* Change CAN state */
    hcan->State = HAL_CAN_STATE_READY;
    
    /* Process unlocked */
    __HAL_UNLOCK(hcan);
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Receives a correct CAN frame.
  * @param  hcan:       Pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @param  FIFONumber: Specify the FIFO number    
  * @retval HAL status
  * @retval None
  */
HAL_StatusTypeDef HAL_CAN_Receive_IT(CAN_HandleTypeDef* hcan, uint8_t FIFONumber)
{
  uint32_t tmp = 0;
  
  /* Check the parameters */
  assert_param(IS_CAN_FIFO(FIFONumber));
  
  tmp = hcan->State;
  if((tmp == HAL_CAN_STATE_READY) || (tmp == HAL_CAN_STATE_BUSY_TX))
  {
    /* Process locked */
    __HAL_LOCK(hcan);
  
    if(hcan->State == HAL_CAN_STATE_BUSY_TX) 
    {
      /* Change CAN state */
      hcan->State = HAL_CAN_STATE_BUSY_TX_RX;
    }
    else
    {
      /* Change CAN state */
      hcan->State = HAL_CAN_STATE_BUSY_RX;
    }
    
    /* Set CAN error code to none */
    hcan->ErrorCode = HAL_CAN_ERROR_NONE;
    
    /* Enable Error warning Interrupt */
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_EWG);
      
    /* Enable Error passive Interrupt */
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_EPV);
      
    /* Enable Bus-off Interrupt */
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_BOF);
      
    /* Enable Last error code Interrupt */
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_LEC);
      
    /* Enable Error Interrupt */
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_ERR);

    /* Process unlocked */
    __HAL_UNLOCK(hcan);

    if(FIFONumber == CAN_FIFO0)
    {
      /* Enable FIFO 0 message pending Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_FMP0);
    }
    else
    {
      /* Enable FIFO 1 message pending Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_FMP1);
    }
    
  }
  else
  {
    return HAL_BUSY;
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Enters the Sleep (low power) mode.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_CAN_Sleep(CAN_HandleTypeDef* hcan)
{
  uint32_t timeout;
   
  /* Process locked */
  __HAL_LOCK(hcan);
  
  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_BUSY; 
    
  /* Request Sleep mode */
   hcan->Instance->MCR = (((hcan->Instance->MCR) & (uint32_t)(~(uint32_t)CAN_MCR_INRQ)) | CAN_MCR_SLEEP);
   
  /* Sleep mode status */
  if ((hcan->Instance->MSR & (CAN_MSR_SLAK|CAN_MSR_INAK)) != CAN_MSR_SLAK)
  {
    /* Return function status */
    return HAL_ERROR;
  }
  
  /* Get timeout */
  timeout = HAL_GetTick() + 10;   
  
  /* Wait the acknowledge */
  while((hcan->Instance->MSR & (CAN_MSR_SLAK|CAN_MSR_INAK)) != CAN_MSR_SLAK)
  {
    if(HAL_GetTick() >= timeout)
    {
      hcan->State = HAL_CAN_STATE_TIMEOUT;
      /* Process unlocked */
      __HAL_UNLOCK(hcan);
      return HAL_TIMEOUT;
    }
  }
  
  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hcan);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Wakes up the CAN peripheral from sleep mode, after that the CAN peripheral
  *         is in the normal mode.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_CAN_WakeUp(CAN_HandleTypeDef* hcan)
{
  uint32_t timeout;
    
  /* Process locked */
  __HAL_LOCK(hcan);
  
  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_BUSY;  
 
  /* Wake up request */
  hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_SLEEP;
    
  /* Get timeout */
  timeout = HAL_GetTick() + 10;   
  
  /* Sleep mode status */
  while((hcan->Instance->MSR & CAN_MSR_SLAK) == CAN_MSR_SLAK)
  {
    if(HAL_GetTick() >= timeout)
    {
      hcan->State= HAL_CAN_STATE_TIMEOUT;
      /* Process unlocked */
      __HAL_UNLOCK(hcan);
      return HAL_TIMEOUT;
    }
  }
  if((hcan->Instance->MSR & CAN_MSR_SLAK) == CAN_MSR_SLAK)
  {
    /* Return function status */
    return HAL_ERROR;
  }
  
  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_READY; 
  
  /* Process unlocked */
  __HAL_UNLOCK(hcan);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Handles CAN interrupt request  
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_IRQHandler(CAN_HandleTypeDef* hcan)
{
  uint32_t tmp1 = 0, tmp2 = 0, tmp3 = 0;
  
  /* Check End of transmission flag */
  if(__HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_TME))
  {
    tmp1 = __HAL_CAN_TRANSMIT_STATUS(hcan, CAN_TXMAILBOX_0);
    tmp2 = __HAL_CAN_TRANSMIT_STATUS(hcan, CAN_TXMAILBOX_1);
    tmp3 = __HAL_CAN_TRANSMIT_STATUS(hcan, CAN_TXMAILBOX_2);
    if(tmp1 || tmp2 || tmp3)  
    {
      /* Call transmit function */
      CAN_Transmit_IT(hcan);
    }
  }
  
  tmp1 = __HAL_CAN_MSG_PENDING(hcan, CAN_FIFO0);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_FMP0);
  /* Check End of reception flag for FIFO0 */
  if((tmp1 != 0) && tmp2)
  {
    /* Call receive function */
    CAN_Receive_IT(hcan, CAN_FIFO0);
  }
  
  tmp1 = __HAL_CAN_MSG_PENDING(hcan, CAN_FIFO1);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_FMP1);
  /* Check End of reception flag for FIFO1 */
  if((tmp1 != 0) && tmp2)
  {
    /* Call receive function */
    CAN_Receive_IT(hcan, CAN_FIFO1);
  }
  
  tmp1 = __HAL_CAN_GET_FLAG(hcan, CAN_FLAG_EWG);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_EWG);
  tmp3 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_ERR);
  /* Check Error Warning Flag */
  if(tmp1 && tmp2 && tmp3)
  {
    /* Set CAN error code to EWG error */
    hcan->ErrorCode |= HAL_CAN_ERROR_EWG;
    /* Clear Error Warning Flag */ 
    __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_EWG);
  }
  
  tmp1 = __HAL_CAN_GET_FLAG(hcan, CAN_FLAG_EPV);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_EPV);
  tmp3 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_ERR); 
  /* Check Error Passive Flag */
  if(tmp1 && tmp2 && tmp3)
  {
    /* Set CAN error code to EPV error */
    hcan->ErrorCode |= HAL_CAN_ERROR_EPV;
    /* Clear Error Passive Flag */ 
    __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_EPV);
  }
  
  tmp1 = __HAL_CAN_GET_FLAG(hcan, CAN_FLAG_BOF);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_BOF);
  tmp3 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_ERR);  
  /* Check Bus-Off Flag */
  if(tmp1 && tmp2 && tmp3)
  {
    /* Set CAN error code to BOF error */
    hcan->ErrorCode |= HAL_CAN_ERROR_BOF;
    /* Clear Bus-Off Flag */ 
    __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_BOF);
  }
  
  tmp1 = HAL_IS_BIT_CLR(hcan->Instance->ESR, CAN_ESR_LEC);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_LEC);
  tmp3 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_ERR);
  /* Check Last error code Flag */
  if((!tmp1) && tmp2 && tmp3)
  {
    tmp1 = (hcan->Instance->ESR) & CAN_ESR_LEC;
    switch(tmp1)
    {
      case(CAN_ESR_LEC_0):
          /* Set CAN error code to STF error */
          hcan->ErrorCode |= HAL_CAN_ERROR_STF;
          break;
      case(CAN_ESR_LEC_1):
          /* Set CAN error code to FOR error */
          hcan->ErrorCode |= HAL_CAN_ERROR_FOR;
          break;
      case(CAN_ESR_LEC_1 | CAN_ESR_LEC_0):
          /* Set CAN error code to ACK error */
          hcan->ErrorCode |= HAL_CAN_ERROR_ACK;
          break;
      case(CAN_ESR_LEC_2):
          /* Set CAN error code to BR error */
          hcan->ErrorCode |= HAL_CAN_ERROR_BR;
          break;
      case(CAN_ESR_LEC_2 | CAN_ESR_LEC_0):
          /* Set CAN error code to BD error */
          hcan->ErrorCode |= HAL_CAN_ERROR_BD;
          break;
      case(CAN_ESR_LEC_2 | CAN_ESR_LEC_1):
          /* Set CAN error code to CRC error */
          hcan->ErrorCode |= HAL_CAN_ERROR_CRC;
          break;
      default:
          break;
    }

    /* Clear Last error code Flag */ 
    hcan->Instance->ESR &= ~(CAN_ESR_LEC);
  }
  
  /* Call the Error call Back in case of Errors */
  if(hcan->ErrorCode != HAL_CAN_ERROR_NONE)
  {
    /* Set the CAN state ready to be able to start again the process */
    hcan->State = HAL_CAN_STATE_READY;
    /* Call Error callback function */
    HAL_CAN_ErrorCallback(hcan);
  }  
}

/**
  * @brief  Transmission  complete callback in non blocking mode 
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Transmission  complete callback in non blocking mode 
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_RxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Error CAN callback.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_ErrorCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup CAN_Group3 Peripheral State and Error functions
 *  @brief   CAN Peripheral State functions 
 *
@verbatim   
  ==============================================================================
            ##### Peripheral State and Error functions #####
  ==============================================================================
    [..]
    This subsection provides functions allowing to
      (+) Check the CAN state.
      (+) Check CAN Errors detected during interrupt process
         
@endverbatim
  * @{
  */

/**
  * @brief  return the CAN state
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL state
  */
HAL_CAN_StateTypeDef HAL_CAN_GetState(CAN_HandleTypeDef* hcan)
{
  /* Return CAN state */
  return hcan->State;
}

/**
  * @brief  Return the CAN error code
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval CAN Error Code
  */
uint32_t HAL_CAN_GetError(CAN_HandleTypeDef *hcan)
{
  return hcan->ErrorCode;
}

/**
  * @}
  */
/**
  * @brief  Initiates and transmits a CAN frame message.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval HAL status
  */
static HAL_StatusTypeDef CAN_Transmit_IT(CAN_HandleTypeDef* hcan)
{
  /* Disable Transmit mailbox empty Interrupt */
  __HAL_CAN_DISABLE_IT(hcan, CAN_IT_TME);
  
  if(hcan->State == HAL_CAN_STATE_BUSY_TX)
  {   
    /* Disable Error warning Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_EWG);
    
    /* Disable Error passive Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_EPV);
    
    /* Disable Bus-off Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_BOF);
    
    /* Disable Last error code Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_LEC);
    
    /* Disable Error Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_ERR);
  }
  
  if(hcan->State == HAL_CAN_STATE_BUSY_TX_RX) 
  {
    /* Change CAN state */
    hcan->State = HAL_CAN_STATE_BUSY_RX;
  }
  else
  {
    /* Change CAN state */
    hcan->State = HAL_CAN_STATE_READY;
  }
  
  /* Transmission complete callback */ 
  HAL_CAN_TxCpltCallback(hcan);
  
  return HAL_OK;
}

/**
  * @brief  Receives a correct CAN frame.
  * @param  hcan:       Pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @param  FIFONumber: Specify the FIFO number    
  * @retval HAL status
  * @retval None
  */
static HAL_StatusTypeDef CAN_Receive_IT(CAN_HandleTypeDef* hcan, uint8_t FIFONumber)
{
  /* Get the Id */
  hcan->pRxMsg->IDE = (uint8_t)0x04 & hcan->Instance->sFIFOMailBox[FIFONumber].RIR;
  if (hcan->pRxMsg->IDE == CAN_ID_STD)
  {
    hcan->pRxMsg->StdId = (uint32_t)0x000007FF & (hcan->Instance->sFIFOMailBox[FIFONumber].RIR >> 21);
  }
  else
  {
    hcan->pRxMsg->ExtId = (uint32_t)0x1FFFFFFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RIR >> 3);
  }
  
  hcan->pRxMsg->RTR = (uint8_t)0x02 & hcan->Instance->sFIFOMailBox[FIFONumber].RIR;
  /* Get the DLC */
  hcan->pRxMsg->DLC = (uint8_t)0x0F & hcan->Instance->sFIFOMailBox[FIFONumber].RDTR;
  /* Get the FMI */
  hcan->pRxMsg->FMI = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDTR >> 8);
  /* Get the data field */
  hcan->pRxMsg->Data[0] = (uint8_t)0xFF & hcan->Instance->sFIFOMailBox[FIFONumber].RDLR;
  hcan->pRxMsg->Data[1] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 8);
  hcan->pRxMsg->Data[2] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 16);
  hcan->pRxMsg->Data[3] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 24);
  hcan->pRxMsg->Data[4] = (uint8_t)0xFF & hcan->Instance->sFIFOMailBox[FIFONumber].RDHR;
  hcan->pRxMsg->Data[5] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 8);
  hcan->pRxMsg->Data[6] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 16);
  hcan->pRxMsg->Data[7] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 24);
  /* Release the FIFO */
  /* Release FIFO0 */
  if (FIFONumber == CAN_FIFO0)
  {
    __HAL_CAN_FIFO_RELEASE(hcan, CAN_FIFO0);
    
    /* Disable FIFO 0 message pending Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_FMP0);
  }
  /* Release FIFO1 */
  else /* FIFONumber == CAN_FIFO1 */
  {
    __HAL_CAN_FIFO_RELEASE(hcan, CAN_FIFO1);
    
    /* Disable FIFO 1 message pending Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_FMP1);
  }
  
  if(hcan->State == HAL_CAN_STATE_BUSY_RX)
  {   
    /* Disable Error warning Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_EWG);
    
    /* Disable Error passive Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_EPV);
    
    /* Disable Bus-off Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_BOF);
    
    /* Disable Last error code Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_LEC);
    
    /* Disable Error Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_ERR);
  }
  
  if(hcan->State == HAL_CAN_STATE_BUSY_TX_RX) 
  {
    /* Disable CAN state */
    hcan->State = HAL_CAN_STATE_BUSY_TX;
  }
  else
  {
    /* Change CAN state */
    hcan->State = HAL_CAN_STATE_READY;
  }

  /* Receive complete callback */ 
  HAL_CAN_RxCpltCallback(hcan);

  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */

#endif /* HAL_CAN_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
