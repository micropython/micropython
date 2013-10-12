/**
  ******************************************************************************
  * @file    usb_otg.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   OTG Core Layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
#include "usb_defines.h"
#include "usb_regs.h"
#include "usb_core.h"
#include "usb_otg.h"

/** @addtogroup USB_OTG_DRIVER
  * @{
  */
  
/** @defgroup USB_OTG 
  * @brief This file is the interface between EFSL ans Host mass-storage class
  * @{
  */


/** @defgroup USB_OTG_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 
 

/** @defgroup USB_OTG_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 



/** @defgroup USB_OTG_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_OTG_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_OTG_Private_FunctionPrototypes
  * @{
  */ 

uint32_t USB_OTG_HandleOTG_ISR(USB_OTG_CORE_HANDLE *pdev);

static uint32_t USB_OTG_HandleConnectorIDStatusChange_ISR(USB_OTG_CORE_HANDLE *pdev);
static uint32_t USB_OTG_HandleSessionRequest_ISR(USB_OTG_CORE_HANDLE *pdev);
static uint32_t USB_OTG_Read_itr(USB_OTG_CORE_HANDLE *pdev);

/**
  * @}
  */ 


/** @defgroup USB_OTG_Private_Functions
  * @{
  */ 


/*                           OTG Interrupt Handler                         */


/**
  * @brief  STM32_USBO_OTG_ISR_Handler
  *         
  * @param  None
  * @retval : None
  */
uint32_t STM32_USBO_OTG_ISR_Handler(USB_OTG_CORE_HANDLE *pdev)
{
  uint32_t retval = 0;
  USB_OTG_GINTSTS_TypeDef  gintsts ;
  gintsts.d32 = 0;

  gintsts.d32 = USB_OTG_Read_itr(pdev);
  if (gintsts.d32 == 0)
  {
    return 0;
  }
  if (gintsts.b.otgintr)
  {
    retval |= USB_OTG_HandleOTG_ISR(pdev);
  }
  if (gintsts.b.conidstschng)
  {
    retval |= USB_OTG_HandleConnectorIDStatusChange_ISR(pdev);
  }
  if (gintsts.b.sessreqintr)
  {
    retval |= USB_OTG_HandleSessionRequest_ISR(pdev);
  }
  return retval;
}


/**
  * @brief  USB_OTG_Read_itr
  *         returns the Core Interrupt register
  * @param  None
  * @retval : status
  */
static uint32_t USB_OTG_Read_itr(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_GINTSTS_TypeDef  gintsts;
  USB_OTG_GINTMSK_TypeDef  gintmsk;
  USB_OTG_GINTMSK_TypeDef  gintmsk_common;
  
  
  gintsts.d32 = 0;
  gintmsk.d32 = 0;
  gintmsk_common.d32 = 0;
  
  /* OTG interrupts */
  gintmsk_common.b.sessreqintr = 1;
  gintmsk_common.b.conidstschng = 1;
  gintmsk_common.b.otgintr = 1;
  
  gintsts.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GINTSTS);
  gintmsk.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GINTMSK);
  return ((gintsts.d32 & gintmsk.d32 ) & gintmsk_common.d32);
}


/**
  * @brief  USB_OTG_HandleOTG_ISR
  *         handles the OTG Interrupts
  * @param  None
  * @retval : status
  */
static uint32_t USB_OTG_HandleOTG_ISR(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_GOTGINT_TypeDef  gotgint;
  USB_OTG_GOTGCTL_TypeDef  gotgctl;
  
  
  gotgint.d32 = 0;
  gotgctl.d32 = 0;
  
  gotgint.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GOTGINT);
  gotgctl.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GOTGCTL);
  
  if (gotgint.b.sesenddet)
  {
    gotgctl.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GOTGCTL);
    
    
    if (USB_OTG_IsDeviceMode(pdev))
    {

    }
    else if (USB_OTG_IsHostMode(pdev))
    {

    }
  }

  /* ----> SRP SUCCESS or FAILURE INTERRUPT <---- */
  if (gotgint.b.sesreqsucstschng)
  {
    gotgctl.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GOTGCTL);
    if (gotgctl.b.sesreqscs) /* Session request success                                          */
    {
      if (USB_OTG_IsDeviceMode(pdev))
      {

      }
      /* Clear Session Request */
      gotgctl.d32 = 0;
      gotgctl.b.sesreq = 1;
      USB_OTG_MODIFY_REG32(&pdev->regs.GREGS->GOTGCTL, gotgctl.d32, 0);
    }
    else /* Session request failure                                          */
    {
      if (USB_OTG_IsDeviceMode(pdev))
      {

      }
    }
  }
  /* ----> HNP SUCCESS or FAILURE INTERRUPT <---- */
  if (gotgint.b.hstnegsucstschng)
  {
    gotgctl.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GOTGCTL);

    if (gotgctl.b.hstnegscs)                                    /* Host negotiation success                                         */
    {
      if (USB_OTG_IsHostMode(pdev))                              /* The core AUTOMATICALLY sets the Host mode                        */
      {

      }
    }
    else                                                        /* Host negotiation failure */
    {

    }
    gotgint.b.hstnegsucstschng = 1;                             /* Ack "Host Negotiation Success Status Change" interrupt.          */
  }
  /* ----> HOST NEGOTIATION DETECTED INTERRUPT <---- */
  if (gotgint.b.hstnegdet)
  {
    if (USB_OTG_IsDeviceMode(pdev))                              /* The core AUTOMATICALLY sets the Host mode                        */
    {

    }
    else
    {

    }
  }
  if (gotgint.b.adevtoutchng)
  {}
  if (gotgint.b.debdone)
  {
    USB_OTG_ResetPort(pdev);
  }
  /* Clear OTG INT */
  USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GOTGINT, gotgint.d32);
  return 1;
}


/**
  * @brief  USB_OTG_HandleConnectorIDStatusChange_ISR
  *         handles the Connector ID Status Change Interrupt
  * @param  None
  * @retval : status
  */
static uint32_t USB_OTG_HandleConnectorIDStatusChange_ISR(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_GINTMSK_TypeDef  gintmsk;
  USB_OTG_GOTGCTL_TypeDef   gotgctl;
  USB_OTG_GINTSTS_TypeDef  gintsts;
  
  gintsts.d32 = 0 ;
  gintmsk.d32 = 0 ;
  gotgctl.d32 = 0 ;
  gintmsk.b.sofintr = 1;
  
  USB_OTG_MODIFY_REG32(&pdev->regs.GREGS->GINTMSK, gintmsk.d32, 0);
  gotgctl.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GOTGCTL);
  
  /* B-Device connector (Device Mode) */
  if (gotgctl.b.conidsts)
  {
    USB_OTG_DisableGlobalInt(pdev);
    USB_OTG_CoreInitDev(pdev);
    USB_OTG_EnableGlobalInt(pdev);
    pdev->otg.OTG_State = B_PERIPHERAL;
  }
  else
  {
    USB_OTG_DisableGlobalInt(pdev);
    USB_OTG_CoreInitHost(pdev);
    USB_OTG_EnableGlobalInt(pdev);
    pdev->otg.OTG_State = A_HOST;
  }
  /* Set flag and clear interrupt */
  gintsts.b.conidstschng = 1;
  USB_OTG_WRITE_REG32 (&pdev->regs.GREGS->GINTSTS, gintsts.d32);
  return 1;
}


/**
  * @brief  USB_OTG_HandleSessionRequest_ISR 
  *           Initiating the Session Request Protocol
  * @param  None
  * @retval : status
  */
static uint32_t USB_OTG_HandleSessionRequest_ISR(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_GINTSTS_TypeDef  gintsts;
  USB_OTG_GOTGCTL_TypeDef   gotgctl;


  gotgctl.d32 = 0;
  gintsts.d32 = 0;

  gotgctl.d32 = USB_OTG_READ_REG32( &pdev->regs.GREGS->GOTGCTL );
  if (USB_OTG_IsDeviceMode(pdev) && (gotgctl.b.bsesvld))
  {

  }
  else if (gotgctl.b.asesvld)
  {
  }
  /* Clear interrupt */
  gintsts.d32 = 0;
  gintsts.b.sessreqintr = 1;
  USB_OTG_WRITE_REG32 (&pdev->regs.GREGS->GINTSTS, gintsts.d32);
  return 1;
}


/**
  * @brief  USB_OTG_InitiateSRP
  *         Initiate an srp session
  * @param  None
  * @retval : None
  */
void USB_OTG_InitiateSRP(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_GOTGCTL_TypeDef  otgctl;

  otgctl.d32 = 0;

  otgctl.d32 = USB_OTG_READ_REG32( &pdev->regs.GREGS->GOTGCTL );
  if (otgctl.b.sesreq)
  {
    return; /* SRP in progress */
  }
  otgctl.b.sesreq = 1;
  USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GOTGCTL, otgctl.d32);
}


/**
  * @brief  USB_OTG_InitiateHNP
  *         Initiate HNP
  * @param  None
  * @retval : None
  */
void USB_OTG_InitiateHNP(USB_OTG_CORE_HANDLE *pdev , uint8_t state, uint8_t mode)
{
  USB_OTG_GOTGCTL_TypeDef   otgctl;
  USB_OTG_HPRT0_TypeDef    hprt0;
  
  otgctl.d32 = 0;
  hprt0.d32  = 0;

  otgctl.d32 = USB_OTG_READ_REG32( &pdev->regs.GREGS->GOTGCTL );
  if (mode)
  { /* Device mode */
    if (state)
    {

      otgctl.b.devhnpen = 1; /* B-Dev has been enabled to perform HNP         */
      otgctl.b.hnpreq   = 1; /* Initiate an HNP req. to the connected USB host*/
      USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GOTGCTL, otgctl.d32);
    }
  }
  else
  { /* Host mode */
    if (state)
    {
      otgctl.b.hstsethnpen = 1; /* A-Dev has enabled B-device for HNP       */
      USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GOTGCTL, otgctl.d32);
      /* Suspend the bus so that B-dev will disconnect indicating the initial condition for HNP to DWC_Core */
      hprt0.d32  = USB_OTG_ReadHPRT0(pdev);
      hprt0.b.prtsusp = 1; /* The core clear this bit when disconnect interrupt generated (GINTSTS.DisconnInt = '1') */
      USB_OTG_WRITE_REG32(pdev->regs.HPRT0, hprt0.d32);
    }
  }
}


/**
  * @brief  USB_OTG_GetCurrentState
  *         Return current OTG State
  * @param  None
  * @retval : None
  */
uint32_t USB_OTG_GetCurrentState (USB_OTG_CORE_HANDLE *pdev)
{
  return pdev->otg.OTG_State;
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
