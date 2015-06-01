/**
  ******************************************************************************
  * @file    stm32f2xx_hal_pcd.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-March-2014
  * @brief   PCD HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the USB Peripheral Controller:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions 
  *           + Peripheral State functions
  *         
  @verbatim
  ==============================================================================
                    ##### How to use this driver #####
  ==============================================================================
    [..]
      The PCD HAL driver can be used as follows:

     (#) Declare a PCD_HandleTypeDef handle structure, for example:
         PCD_HandleTypeDef  hpcd;
        
     (#) Fill parameters of Init structure in HCD handle
  
     (#) Call HAL_PCD_Init() API to initialize the HCD peripheral (Core, Device core, ...) 

     (#) Initialize the PCD low level resources through the HAL_PCD_MspInit() API:
         (##) Enable the PCD/USB Low Level interface clock using 
              (+++) __OTGFS-OTG_CLK_ENABLE()/__OTGHS-OTG_CLK_ENABLE();
              (+++) __OTGHSULPI_CLK_ENABLE(); (For High Speed Mode)
           
         (##) Initialize the related GPIO clocks
         (##) Configure PCD pin-out
         (##) Configure PCD NVIC interrupt
    
     (#)Associate the Upper USB device stack to the HAL PCD Driver:
         (##) hpcd.pData = pdev;

     (#)Enable HCD transmission and reception:
         (##) HAL_PCD_Start();

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
#include "stm32f2xx_hal.h"

/** @addtogroup STM32F2xx_HAL_Driver
  * @{
  */

/** @defgroup PCD 
  * @brief PCD HAL module driver
  * @{
  */

#ifdef HAL_PCD_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define PCD_MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define PCD_MAX(a, b)  (((a) > (b)) ? (a) : (b))
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef PCD_WriteEmptyTxFifo(PCD_HandleTypeDef *hpcd, uint32_t epnum);
/* Private functions ---------------------------------------------------------*/

/** @defgroup PCD_Private_Functions
  * @{
  */

/** @defgroup PCD_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
            ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
 
@endverbatim
  * @{
  */

/**
  * @brief  Initializes the PCD according to the specified
  *         parameters in the PCD_InitTypeDef and create the associated handle.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd)
{ 
  uint32_t i = 0;
  
  /* Check the PCD handle allocation */
  if(hpcd == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_PCD_ALL_INSTANCE(hpcd->Instance));

  hpcd->State = PCD_BUSY;
  
  /* Init the low level hardware : GPIO, CLOCK, NVIC... */
  HAL_PCD_MspInit(hpcd);

  /* Disable the Interrupts */
 __HAL_PCD_DISABLE(hpcd);
 
 /*Init the Core (common init.) */
 USB_CoreInit(hpcd->Instance, hpcd->Init);
 
 /* Force Device Mode*/
 USB_SetCurrentMode(hpcd->Instance , USB_OTG_DEVICE_MODE);
 
 /* Init endpoints structures */
 for (i = 0; i < 15 ; i++)
 {
   /* Init ep structure */
   hpcd->IN_ep[i].is_in = 1;
   hpcd->IN_ep[i].num = i;
   hpcd->IN_ep[i].tx_fifo_num = i;
   /* Control until ep is actvated */
   hpcd->IN_ep[i].type = EP_TYPE_CTRL;
   hpcd->IN_ep[i].maxpacket =  0;
   hpcd->IN_ep[i].xfer_buff = 0;
   hpcd->IN_ep[i].xfer_len = 0;
 }
 
 for (i = 0; i < 15 ; i++)
 {
   hpcd->OUT_ep[i].is_in = 0;
   hpcd->OUT_ep[i].num = i;
   hpcd->IN_ep[i].tx_fifo_num = i;
   /* Control until ep is activated */
   hpcd->OUT_ep[i].type = EP_TYPE_CTRL;
   hpcd->OUT_ep[i].maxpacket = 0;
   hpcd->OUT_ep[i].xfer_buff = 0;
   hpcd->OUT_ep[i].xfer_len = 0;
   
   hpcd->Instance->DIEPTXF[i] = 0;
 }
 
 /* Init Device */
 USB_DevInit(hpcd->Instance, hpcd->Init);
 
 hpcd->State= PCD_READY;
 
 USB_DevDisconnect (hpcd->Instance);  
 return HAL_OK;
}

/**
  * @brief  DeInitializes the PCD peripheral 
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd)
{
  /* Check the PCD handle allocation */
  if(hpcd == NULL)
  {
    return HAL_ERROR;
  }

  hpcd->State = PCD_BUSY;
  
  /* Stop Device */
  HAL_PCD_Stop(hpcd);
    
  /* DeInit the low level hardware */
  HAL_PCD_MspDeInit(hpcd);
  
  hpcd->State = PCD_READY; 
  
  return HAL_OK;
}

/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_MspInit could be implenetd in the user file
   */
}

/**
  * @brief  DeInitializes PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_MspDeInit could be implenetd in the user file
   */
}

/**
  * @}
  */

/** @defgroup PCD_Group2 IO operation functions 
 *  @brief   Data transfers functions 
 *
@verbatim   
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to manage the PCD data 
    transfers.

@endverbatim
  * @{
  */
  
/**
  * @brief  Start The USB OTG Device.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd)
{ 
  __HAL_LOCK(hpcd); 
  USB_DevConnect (hpcd->Instance);  
  __HAL_PCD_ENABLE(hpcd);
  __HAL_UNLOCK(hpcd); 
  return HAL_OK;
}

/**
  * @brief  Stop The USB OTG Device.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd)
{ 
  __HAL_LOCK(hpcd); 
  __HAL_PCD_DISABLE(hpcd);
  USB_StopDevice(hpcd->Instance);
  USB_DevDisconnect (hpcd->Instance);
  __HAL_UNLOCK(hpcd); 
  return HAL_OK;
}

/**
  * @brief  This function handles PCD interrupt request.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
  uint32_t i = 0, ep_intr = 0, epint = 0, epnum = 0;
  uint32_t fifoemptymsk = 0, temp = 0;
  USB_OTG_EPTypeDef *ep;
    
  /* ensure that we are in device mode */
  if (USB_GetMode(hpcd->Instance) == USB_OTG_MODE_DEVICE)
  {
    /* avoid spurious interrupt */
    if(__HAL_IS_INVALID_INTERRUPT(hpcd)) 
    {
      return;
    }
    
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_MMIS))
    {
     /* incorrect mode, acknowledge the interrupt */
      __HAL_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_MMIS);
    }
    
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_OEPINT))
    {
      epnum = 0;
      
      /* Read in the device interrupt bits */
      ep_intr = USB_ReadDevAllOutEpInterrupt(hpcd->Instance);
      
      while ( ep_intr )
      {
        if (ep_intr & 0x1)
        {
          epint = USB_ReadDevOutEPInterrupt(hpcd->Instance, epnum);
          
          if(( epint & USB_OTG_DOEPINT_XFRC) == USB_OTG_DOEPINT_XFRC)
          {
            CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_XFRC);
            
            if(hpcd->Init.dma_enable == 1)
            {
              hpcd->OUT_ep[epnum].xfer_count = hpcd->OUT_ep[epnum].maxpacket- (USBx_OUTEP(epnum)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ); 
              hpcd->OUT_ep[epnum].xfer_buff += hpcd->OUT_ep[epnum].maxpacket;            
            }
            
            HAL_PCD_DataOutStageCallback(hpcd, epnum);
            if(hpcd->Init.dma_enable == 1)
            {
              if((epnum == 0) && (hpcd->OUT_ep[epnum].xfer_len == 0))
              {
                 /* this is ZLP, so prepare EP0 for next setup */
                USB_EP0_OutStart(hpcd->Instance, 1, (uint8_t *)hpcd->Setup);
              }              
            }
          }
          
          if(( epint & USB_OTG_DOEPINT_STUP) == USB_OTG_DOEPINT_STUP)
          {
            /* Inform the upper layer that a setup packet is available */
            HAL_PCD_SetupStageCallback(hpcd);
            CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_STUP);
          }
          
          if(( epint & USB_OTG_DOEPINT_OTEPDIS) == USB_OTG_DOEPINT_OTEPDIS)
          {
            CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPDIS);
          }
        }
        epnum++;
        ep_intr >>= 1;
      }
    }
    
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_IEPINT))
    {
      /* Read in the device interrupt bits */
      ep_intr = USB_ReadDevAllInEpInterrupt(hpcd->Instance);
      
      epnum = 0;
      
      while ( ep_intr )
      {
        if (ep_intr & 0x1) /* In ITR */
        {
          epint = USB_ReadDevInEPInterrupt(hpcd->Instance, epnum);

           if(( epint & USB_OTG_DIEPINT_XFRC) == USB_OTG_DIEPINT_XFRC)
          {
            fifoemptymsk = 0x1 << epnum;
            USBx_DEVICE->DIEPEMPMSK = ~fifoemptymsk;
            
            CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_XFRC);
            
            if (hpcd->Init.dma_enable == 1)
            {
              hpcd->IN_ep[epnum].xfer_buff += hpcd->IN_ep[epnum].maxpacket; 
            }
                                      
            HAL_PCD_DataInStageCallback(hpcd, epnum);

            if (hpcd->Init.dma_enable == 1)
            {
              /* this is ZLP, so prepare EP0 for next setup */
              if((epnum == 0) && (hpcd->IN_ep[epnum].xfer_len == 0))
              {
                /* prepare to rx more setup packets */
                USB_EP0_OutStart(hpcd->Instance, 1, (uint8_t *)hpcd->Setup);
              }
            }           
          }
           if(( epint & USB_OTG_DIEPINT_TOC) == USB_OTG_DIEPINT_TOC)
          {
            CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_TOC);
          }
          if(( epint & USB_OTG_DIEPINT_ITTXFE) == USB_OTG_DIEPINT_ITTXFE)
          {
            CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_ITTXFE);
          }
          if(( epint & USB_OTG_DIEPINT_INEPNE) == USB_OTG_DIEPINT_INEPNE)
          {
            CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_INEPNE);
          }
          if(( epint & USB_OTG_DIEPINT_EPDISD) == USB_OTG_DIEPINT_EPDISD)
          {
            CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_EPDISD);
          }       
          if(( epint & USB_OTG_DIEPINT_TXFE) == USB_OTG_DIEPINT_TXFE)
          {
            PCD_WriteEmptyTxFifo(hpcd , epnum);
          }
        }
        epnum++;
        ep_intr >>= 1;
      }
    }
    
    /* Handle Resume Interrupt */
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_WKUINT))
    {    
     /* Clear the Remote Wake-up Signaling */
      USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;
     
     HAL_PCD_ResumeCallback(hpcd);

     __HAL_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_WKUINT);
    }
    
    /* Handle Suspend Interrupt */
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_USBSUSP))
    {

      if((USBx_DEVICE->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS)
      { 
        HAL_PCD_SuspendCallback(hpcd);
      }
      __HAL_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_USBSUSP);
    }
    
    /* Handle Reset Interrupt */
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_USBRST))
    {
      USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG; 
      USB_FlushTxFifo(hpcd->Instance ,  0 );
      
      for (i = 0; i < hpcd->Init.dev_endpoints ; i++)
      {
        USBx_INEP(i)->DIEPINT = 0xFF;
        USBx_OUTEP(i)->DOEPINT = 0xFF;
      }
      USBx_DEVICE->DAINT = 0xFFFFFFFF;
      USBx_DEVICE->DAINTMSK |= 0x10001;
      
      if(hpcd->Init.use_dedicated_ep1)
      {
        USBx_DEVICE->DOUTEP1MSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM); 
        USBx_DEVICE->DINEP1MSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);  
      }
      else
      {
        USBx_DEVICE->DOEPMSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM);
        USBx_DEVICE->DIEPMSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);
      }
      
      /* Set Default Address to 0 */
      USBx_DEVICE->DCFG &= ~USB_OTG_DCFG_DAD;
      
      /* setup EP0 to receive SETUP packets */
      USB_EP0_OutStart(hpcd->Instance, hpcd->Init.dma_enable, (uint8_t *)hpcd->Setup);
        
      __HAL_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_USBRST);
    }
    
    /* Handle Enumeration done Interrupt */
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_ENUMDNE))
    {
      USB_ActivateSetup(hpcd->Instance);
      hpcd->Instance->GUSBCFG &= ~USB_OTG_GUSBCFG_TRDT;
      
      if ( USB_GetDevSpeed(hpcd->Instance) == USB_OTG_SPEED_HIGH)
      {
        hpcd->Init.speed            = USB_OTG_SPEED_HIGH;
        hpcd->Init.ep0_mps          = USB_OTG_HS_MAX_PACKET_SIZE ;    
        hpcd->Instance->GUSBCFG |= (USB_OTG_GUSBCFG_TRDT_0 | USB_OTG_GUSBCFG_TRDT_3);
      }
      else
      {
        hpcd->Init.speed            = USB_OTG_SPEED_FULL;
        hpcd->Init.ep0_mps          = USB_OTG_FS_MAX_PACKET_SIZE ;  
        hpcd->Instance->GUSBCFG |= (USB_OTG_GUSBCFG_TRDT_0 | USB_OTG_GUSBCFG_TRDT_2);
      }
      
      HAL_PCD_ResetCallback(hpcd);
      
      __HAL_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_ENUMDNE);
    }
    
     
    /* Handle RxQLevel Interrupt */
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_RXFLVL))
    {
      USB_MASK_INTERRUPT(hpcd->Instance, USB_OTG_GINTSTS_RXFLVL);
      temp = USBx->GRXSTSP;
      ep = &hpcd->OUT_ep[temp & USB_OTG_GRXSTSP_EPNUM];
      
      if(((temp & USB_OTG_GRXSTSP_PKTSTS) >> 17) ==  STS_DATA_UPDT)
      {
        if((temp & USB_OTG_GRXSTSP_BCNT) != 0)
        {
          USB_ReadPacket(USBx, ep->xfer_buff, (temp & USB_OTG_GRXSTSP_BCNT) >> 4);
          ep->xfer_buff += (temp & USB_OTG_GRXSTSP_BCNT) >> 4;
          ep->xfer_count += (temp & USB_OTG_GRXSTSP_BCNT) >> 4;
        }
      }
      else if (((temp & USB_OTG_GRXSTSP_PKTSTS) >> 17) ==  STS_SETUP_UPDT)
      {
        USB_ReadPacket(USBx, (uint8_t *)hpcd->Setup, 8);
        ep->xfer_count += (temp & USB_OTG_GRXSTSP_BCNT) >> 4;
      }
      USB_UNMASK_INTERRUPT(hpcd->Instance, USB_OTG_GINTSTS_RXFLVL);
    }
    
    /* Handle SOF Interrupt */
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_SOF))
    {
      HAL_PCD_SOFCallback(hpcd);
      __HAL_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_SOF);
    }
    
    /* Handle Incomplete ISO IN Interrupt */
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_IISOIXFR))
    {
      HAL_PCD_ISOINIncompleteCallback(hpcd, epnum);
      __HAL_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_IISOIXFR);
    } 
    
    /* Handle Incomplete ISO OUT Interrupt */
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT))
    {
      HAL_PCD_ISOOUTIncompleteCallback(hpcd, epnum);
      __HAL_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT);
    } 
    
    /* Handle Connection event Interrupt */
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_SRQINT))
    {
      HAL_PCD_ConnectCallback(hpcd);
      __HAL_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_SRQINT);
    } 
    
    /* Handle Disconnection event Interrupt */
    if(__HAL_GET_FLAG(hpcd, USB_OTG_GINTSTS_OTGINT))
    {
      temp = hpcd->Instance->GOTGINT;
      
      if((temp & USB_OTG_GOTGINT_SEDET) == USB_OTG_GOTGINT_SEDET)
      {
        HAL_PCD_DisconnectCallback(hpcd);
      }
      hpcd->Instance->GOTGINT |= temp;
    }
  }
}

/**
  * @brief  Data out stage callbacks
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implenetd in the user file
   */ 
}

/**
  * @brief  Data IN stage callbacks
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implenetd in the user file
   */ 
}
/**
  * @brief  Setup stage callback
  * @param  hpcd: ppp handle
  * @retval None
  */
 __weak void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implenetd in the user file
   */ 
}

/**
  * @brief  USB Start Of Frame callbacks
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implenetd in the user file
   */ 
}

/**
  * @brief  USB Reset callbacks
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implenetd in the user file
   */ 
}

/**
  * @brief  Suspend event callbacks
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implenetd in the user file
   */ 
}

/**
  * @brief  Resume event callbacks
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implenetd in the user file
   */ 
}

/**
  * @brief  Incomplete ISO OUT callbacks
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implenetd in the user file
   */ 
}

/**
  * @brief  Incomplete ISO IN  callbacks
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implenetd in the user file
   */ 
}

/**
  * @brief  Connection event callbacks
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implenetd in the user file
   */ 
}

/**
  * @brief  Disconnection event callbacks
  * @param  hpcd: ppp handle
  * @retval None
  */
 __weak void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implenetd in the user file
   */ 
}

/**
  * @}
  */
  
/** @defgroup PCD_Group3 Peripheral Control functions 
 *  @brief   management functions 
 *
@verbatim   
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to control the PCD data 
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Send an amount of data in blocking mode 
  * @param  hpcd: PCD handle
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DevConnect(PCD_HandleTypeDef *hpcd)
{
  __HAL_LOCK(hpcd); 
  USB_DevConnect(hpcd->Instance);
  __HAL_UNLOCK(hpcd); 
  return HAL_OK;
}

/**
  * @brief  Send an amount of data in blocking mode 
  * @param  hpcd: PCD handle
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DevDisconnect(PCD_HandleTypeDef *hpcd)
{
  __HAL_LOCK(hpcd); 
  USB_DevDisconnect(hpcd->Instance);
  __HAL_UNLOCK(hpcd); 
  return HAL_OK;
}

/**
  * @brief  Set the USB Device address 
  * @param  hpcd: PCD handle
  * @param  address: new device address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address)
{
  __HAL_LOCK(hpcd); 
  USB_SetDevAddress(hpcd->Instance, address);
  __HAL_UNLOCK(hpcd);   
  return HAL_OK;
}
/**
  * @brief  Open and configure an endpoint
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  ep_mps: endpoint max packert size
  * @param  ep_type: endpoint type   
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type)
{
  HAL_StatusTypeDef  ret = HAL_OK;
  USB_OTG_EPTypeDef *ep;
  
  if ((ep_addr & 0x80) == 0x80)
  {
    ep = &hpcd->IN_ep[ep_addr & 0x7F];
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr & 0x7F];
  }
  ep->num   = ep_addr & 0x7F;
  
  ep->is_in = (0x80 & ep_addr) != 0;
  ep->maxpacket = ep_mps;
  ep->type = ep_type;
  if (ep->is_in)
  {
    /* Assign a Tx FIFO */
    ep->tx_fifo_num = ep->num;
  }
  /* Set initial data PID. */
  if (ep_type == EP_TYPE_BULK )
  {
    ep->data_pid_start = 0;
  }
  
  __HAL_LOCK(hpcd); 
  USB_ActivateEndpoint(hpcd->Instance , ep);
  __HAL_UNLOCK(hpcd);   
  return ret;
}


/**
  * @brief  Deactivate an endpoint
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{  
  USB_OTG_EPTypeDef *ep;
  
  if ((ep_addr & 0x80) == 0x80)
  {
    ep = &hpcd->IN_ep[ep_addr & 0x7F];
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr & 0x7F];
  }
  ep->num   = ep_addr & 0x7F;
  
  ep->is_in = (0x80 & ep_addr) != 0;
  
  __HAL_LOCK(hpcd); 
  USB_DeactivateEndpoint(hpcd->Instance , ep);
  __HAL_UNLOCK(hpcd);   
  return HAL_OK;
}


/**
  * @brief  Receive an amount of data  
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  pBuf: pointer to the reception buffer   
  * @param  len: amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
  
  USB_OTG_EPTypeDef *ep;
  
  ep = &hpcd->OUT_ep[ep_addr & 0x7F];
  
  /*setup and start the Xfer */
  ep->xfer_buff = pBuf;  
  ep->xfer_len = len;
  ep->xfer_count = 0;
  ep->is_in = 0;
  ep->num = ep_addr & 0x7F;
  
  if (hpcd->Init.dma_enable == 1)
  {
    ep->dma_addr = (uint32_t)pBuf;  
  }
  
  __HAL_LOCK(hpcd); 
  
  if ((ep_addr & 0x7F) == 0 )
  {
    USB_EP0StartXfer(hpcd->Instance , ep, hpcd->Init.dma_enable);
  }
  else
  {
    USB_EPStartXfer(hpcd->Instance , ep, hpcd->Init.dma_enable);
  }
  __HAL_UNLOCK(hpcd); 
  
  return HAL_OK;
}

/**
  * @brief  Get Received Data Size
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval Data Size
  */
uint16_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  return hpcd->OUT_ep[ep_addr & 0x7F].xfer_count;
}
/**
  * @brief  Send an amount of data  
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  pBuf: pointer to the transmission buffer   
  * @param  len: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
  USB_OTG_EPTypeDef *ep;
  
  ep = &hpcd->IN_ep[ep_addr & 0x7F];
  
  /*setup and start the Xfer */
  ep->xfer_buff = pBuf;  
  ep->xfer_len = len;
  ep->xfer_count = 0;
  ep->is_in = 1;
  ep->num = ep_addr & 0x7F;
  
  if (hpcd->Init.dma_enable == 1)
  {
    ep->dma_addr = (uint32_t)pBuf;  
  }
  
  __HAL_LOCK(hpcd); 
  
  if ((ep_addr & 0x7F) == 0 )
  {
    USB_EP0StartXfer(hpcd->Instance , ep, hpcd->Init.dma_enable);
  }
  else
  {
    USB_EPStartXfer(hpcd->Instance , ep, hpcd->Init.dma_enable);
  }
  
  __HAL_UNLOCK(hpcd);
     
  return HAL_OK;
}

/**
  * @brief  Set a STALL condition over an endpoint
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  USB_OTG_EPTypeDef *ep;
  
  if ((0x80 & ep_addr) == 0x80)
  {
    ep = &hpcd->IN_ep[ep_addr & 0x7F];
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr];
  }
  
  ep->is_stall = 1;
  ep->num   = ep_addr & 0x7F;
  ep->is_in = ((ep_addr & 0x80) == 0x80);
  
  
  __HAL_LOCK(hpcd); 
  USB_EPSetStall(hpcd->Instance , ep);
  if((ep_addr & 0x7F) == 0)
  {
    USB_EP0_OutStart(hpcd->Instance, hpcd->Init.dma_enable, (uint8_t *)hpcd->Setup);
  }
  __HAL_UNLOCK(hpcd); 
  
  return HAL_OK;
}

/**
  * @brief  Clear a STALL condition over in an endpoint
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  USB_OTG_EPTypeDef *ep;
  
  if ((0x80 & ep_addr) == 0x80)
  {
    ep = &hpcd->IN_ep[ep_addr & 0x7F];
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr];
  }
  
  ep->is_stall = 0;
  ep->num   = ep_addr & 0x7F;
  ep->is_in = ((ep_addr & 0x80) == 0x80);
  
  __HAL_LOCK(hpcd); 
  USB_EPClearStall(hpcd->Instance , ep);
  __HAL_UNLOCK(hpcd); 
    
  return HAL_OK;
}

/**
  * @brief  Flush an endpoint
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  __HAL_LOCK(hpcd); 
  
  if ((ep_addr & 0x80) == 0x80)
  {
    USB_FlushTxFifo(hpcd->Instance, ep_addr & 0x7F);
  }
  else
  {
    USB_FlushRxFifo(hpcd->Instance);
  }
  
  __HAL_UNLOCK(hpcd); 
    
  return HAL_OK;
}

/**
  * @brief  Update FIFO configuration
  * @param  hpcd: PCD handle
  * @retval status
  */
HAL_StatusTypeDef HAL_PCD_SetTxFiFo(PCD_HandleTypeDef *hpcd, uint8_t fifo, uint16_t size)
{
  uint8_t i = 0;
  uint32_t Tx_Offset = 0;
  
  
  /*  TXn min size = 16 words. (n  : Transmit FIFO index)
  *   When a TxFIFO is not used, the Configuration should be as follows: 
  *       case 1 :  n > m    and Txn is not used    (n,m  : Transmit FIFO indexes)
  *       --> Txm can use the space allocated for Txn.
  *       case2  :  n < m    and Txn is not used    (n,m  : Transmit FIFO indexes)
  *       --> Txn should be configured with the minimum space of 16 words
  *  The FIFO is used optimally when used TxFIFOs are allocated in the top 
  *       of the FIFO.Ex: use EP1 and EP2 as IN instead of EP1 and EP3 as IN ones.
  *   When DMA is used 3n * FIFO locations should be reserved for internal DMA registers */
  
  Tx_Offset = hpcd->Instance->GRXFSIZ;
  
  if(fifo == 0)
  {
    hpcd->Instance->DIEPTXF0_HNPTXFSIZ = (size << 16) | Tx_Offset;
  }
  else
  {
    Tx_Offset += (hpcd->Instance->DIEPTXF0_HNPTXFSIZ) >> 16;
    for (i = 0; i < (fifo - 1); i++)
    {
      Tx_Offset += (hpcd->Instance->DIEPTXF[i] >> 16);
    }
    
    /* Multiply Tx_Size by 2 to get higher performance */
    hpcd->Instance->DIEPTXF[fifo - 1] = (size << 16) | Tx_Offset;
    
  }
  
  return HAL_OK;
}

/**
  * @brief  Update FIFO configuration
  * @param  hpcd: PCD handle
  * @retval status
  */
HAL_StatusTypeDef HAL_PCD_SetRxFiFo(PCD_HandleTypeDef *hpcd, uint16_t size)
{
  
  hpcd->Instance->GRXFSIZ = size;
  
  return HAL_OK;
}


/**
  * @brief  HAL_PCD_ActiveRemoteWakeup : active remote wakeup signalling
  * @param  hpcd: PCD handle
  * @retval status
  */
HAL_StatusTypeDef HAL_PCD_ActiveRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;  
    
  if((USBx_DEVICE->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS)
  {
    /* active Remote wakeup signaling */
    USBx_DEVICE->DCTL |= USB_OTG_DCTL_RWUSIG;
  }
  return HAL_OK;  
}

/**
  * @brief  HAL_PCD_DeActiveRemoteWakeup : de-active remote wakeup signalling
  * @param  hpcd: PCD handle
  * @retval status
  */
HAL_StatusTypeDef HAL_PCD_DeActiveRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;  
  
  /* active Remote wakeup signaling */
   USBx_DEVICE->DCTL &= ~(USB_OTG_DCTL_RWUSIG);
  return HAL_OK;  
}
/**
  * @}
  */
  
/** @defgroup PCD_Group4 Peripheral State functions 
 *  @brief   Peripheral State functions 
 *
@verbatim   
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================  
    [..]
    This subsection permit to get in run-time the status of the peripheral 
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Return the PCD state
  * @param  hpcd : PCD handle
  * @retval HAL state
  */
PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef *hpcd)
{
  return hpcd->State;
}
/**
  * @}
  */

/**
  * @brief  DCD_WriteEmptyTxFifo
  *         check FIFO for the next packet to be loaded
  * @param  hpcd: PCD handle
  * @retval status
  */
static HAL_StatusTypeDef PCD_WriteEmptyTxFifo(PCD_HandleTypeDef *hpcd, uint32_t epnum)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;  
  USB_OTG_EPTypeDef *ep;
  int32_t len = 0;
  uint32_t len32b;
  uint32_t fifoemptymsk = 0;

  ep = &hpcd->IN_ep[epnum];
  len = ep->xfer_len - ep->xfer_count;
  
  if (len > ep->maxpacket)
  {
    len = ep->maxpacket;
  }
  
  len32b = (len + 3) / 4;
 
  while  ( (USBx_INEP(epnum)->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) > len32b &&
          ep->xfer_count < ep->xfer_len &&
            ep->xfer_len != 0)
  {
    /* Write the FIFO */
    len = ep->xfer_len - ep->xfer_count;
    
    if (len > ep->maxpacket)
    {
      len = ep->maxpacket;
    }
    len32b = (len + 3) / 4;
    
    USB_WritePacket(USBx, ep->xfer_buff, epnum, len, hpcd->Init.dma_enable); 
    
    ep->xfer_buff  += len;
    ep->xfer_count += len;
  }
  
  if(len <= 0)
  {
    fifoemptymsk = 0x1 << epnum;
    USBx_DEVICE->DIEPEMPMSK &= ~fifoemptymsk;   
  }
  
  return HAL_OK;  
}

/**
  * @}
  */

#endif /* HAL_PCD_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
