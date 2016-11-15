/**
  ******************************************************************************
  * @file    stm32f4xx_hal_pcd.c
  * @author  MCD Application Team
  * @version V1.5.2
  * @date    22-September-2016
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
  
     (#) Call HAL_PCD_Init() API to initialize the PCD peripheral (Core, Device core, ...) 

     (#) Initialize the PCD low level resources through the HAL_PCD_MspInit() API:
         (##) Enable the PCD/USB Low Level interface clock using 
              (+++) __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
              (+++) __HAL_RCC_USB_OTG_HS_CLK_ENABLE(); (For High Speed Mode)
           
         (##) Initialize the related GPIO clocks
         (##) Configure PCD pin-out
         (##) Configure PCD NVIC interrupt
    
     (#)Associate the Upper USB device stack to the HAL PCD Driver:
         (##) hpcd.pData = pdev;

     (#)Enable PCD transmission and reception:
         (##) HAL_PCD_Start();

  @endverbatim
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
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup PCD PCD
  * @brief PCD HAL module driver
  * @{
  */

#ifdef HAL_PCD_MODULE_ENABLED
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || \
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || \
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F446xx) || \
    defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || \
    defined(STM32F412Rx) || defined(STM32F412Cx)
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @defgroup PCD_Private_Macros PCD Private Macros
  * @{
  */ 
#define PCD_MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define PCD_MAX(a, b)  (((a) > (b)) ? (a) : (b))
/**
  * @}
  */

/* Private functions prototypes ----------------------------------------------*/
/** @defgroup PCD_Private_Functions PCD Private Functions
  * @{
  */
static HAL_StatusTypeDef PCD_WriteEmptyTxFifo(PCD_HandleTypeDef *hpcd, uint32_t epnum);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup PCD_Exported_Functions PCD Exported Functions
  * @{
  */

/** @defgroup PCD_Exported_Functions_Group1 Initialization and de-initialization functions 
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
  *         parameters in the PCD_InitTypeDef and initialize the associated handle.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd)
{ 
  uint32_t i = 0U;
  
  /* Check the PCD handle allocation */
  if(hpcd == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_PCD_ALL_INSTANCE(hpcd->Instance));

  hpcd->State = HAL_PCD_STATE_BUSY;
  
  /* Init the low level hardware : GPIO, CLOCK, NVIC... */
  HAL_PCD_MspInit(hpcd);

  /* Disable the Interrupts */
 __HAL_PCD_DISABLE(hpcd);
 
 /*Init the Core (common init.) */
 USB_CoreInit(hpcd->Instance, hpcd->Init);
 
 /* Force Device Mode*/
 USB_SetCurrentMode(hpcd->Instance , USB_OTG_DEVICE_MODE);
 
 /* Init endpoints structures */
 for (i = 0U; i < 15U; i++)
 {
   /* Init ep structure */
   hpcd->IN_ep[i].is_in = 1U;
   hpcd->IN_ep[i].num = i;
   hpcd->IN_ep[i].tx_fifo_num = i;
   /* Control until ep is activated */
   hpcd->IN_ep[i].type = EP_TYPE_CTRL;
   hpcd->IN_ep[i].maxpacket = 0U;
   hpcd->IN_ep[i].xfer_buff = 0U;
   hpcd->IN_ep[i].xfer_len = 0U;
 }
 
 for (i = 0U; i < 15U; i++)
 {
   hpcd->OUT_ep[i].is_in = 0U;
   hpcd->OUT_ep[i].num = i;
   hpcd->IN_ep[i].tx_fifo_num = i;
   /* Control until ep is activated */
   hpcd->OUT_ep[i].type = EP_TYPE_CTRL;
   hpcd->OUT_ep[i].maxpacket = 0U;
   hpcd->OUT_ep[i].xfer_buff = 0U;
   hpcd->OUT_ep[i].xfer_len = 0U;
   
   hpcd->Instance->DIEPTXF[i] = 0U;
 }
 
 /* Init Device */
 USB_DevInit(hpcd->Instance, hpcd->Init);
 
 hpcd->State= HAL_PCD_STATE_READY;
 
#ifdef USB_OTG_GLPMCFG_LPMEN
 /* Activate LPM */
 if (hpcd->Init.lpm_enable == 1U)
 {
   HAL_PCDEx_ActivateLPM(hpcd);
 }
#endif /* USB_OTG_GLPMCFG_LPMEN */
 
#ifdef USB_OTG_GCCFG_BCDEN
 /* Activate Battery charging */
 if (hpcd->Init.battery_charging_enable == 1U)
 {
   HAL_PCDEx_ActivateBCD(hpcd);
 }
#endif /* USB_OTG_GCCFG_BCDEN */
 
 USB_DevDisconnect (hpcd->Instance);  
 return HAL_OK;
}

/**
  * @brief  DeInitializes the PCD peripheral. 
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

  hpcd->State = HAL_PCD_STATE_BUSY;
  
  /* Stop Device */
  HAL_PCD_Stop(hpcd);
    
  /* DeInit the low level hardware */
  HAL_PCD_MspDeInit(hpcd);
  
  hpcd->State = HAL_PCD_STATE_RESET; 
  
  return HAL_OK;
}

/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup PCD_Exported_Functions_Group2 Input and Output operation functions 
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
  USB_DevDisconnect(hpcd->Instance);
  __HAL_UNLOCK(hpcd); 
  return HAL_OK;
}

/**
  * @brief  Handles PCD interrupt request.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
  uint32_t i = 0U, ep_intr = 0U, epint = 0U, epnum = 0U;
  uint32_t fifoemptymsk = 0U, temp = 0U;
  USB_OTG_EPTypeDef *ep;
  uint32_t hclk = 180000000;
  
  /* ensure that we are in device mode */
  if (USB_GetMode(hpcd->Instance) == USB_OTG_MODE_DEVICE)
  {    
    /* avoid spurious interrupt */
    if(__HAL_PCD_IS_INVALID_INTERRUPT(hpcd)) 
    {
      return;
    }
    
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_MMIS))
    {
     /* incorrect mode, acknowledge the interrupt */
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_MMIS);
    }
    
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_OEPINT))
    {
      epnum = 0U;
      
      /* Read in the device interrupt bits */
      ep_intr = USB_ReadDevAllOutEpInterrupt(hpcd->Instance);
      
      while ( ep_intr )
      {
        if (ep_intr & 0x1U)
        {
          epint = USB_ReadDevOutEPInterrupt(hpcd->Instance, epnum);
          
          if(( epint & USB_OTG_DOEPINT_XFRC) == USB_OTG_DOEPINT_XFRC)
          {
            CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_XFRC);
            
            if(hpcd->Init.dma_enable == 1U)
            {
              hpcd->OUT_ep[epnum].xfer_count = hpcd->OUT_ep[epnum].maxpacket- (USBx_OUTEP(epnum)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ); 
              hpcd->OUT_ep[epnum].xfer_buff += hpcd->OUT_ep[epnum].maxpacket;            
            }
            
            HAL_PCD_DataOutStageCallback(hpcd, epnum);
            if(hpcd->Init.dma_enable == 1U)
            {
              if((epnum == 0U) && (hpcd->OUT_ep[epnum].xfer_len == 0U))
              {
                 /* this is ZLP, so prepare EP0 for next setup */
                USB_EP0_OutStart(hpcd->Instance, 1U, (uint8_t *)hpcd->Setup);
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

#ifdef USB_OTG_DOEPINT_OTEPSPR 
          /* Clear Status Phase Received interrupt */
          if(( epint & USB_OTG_DOEPINT_OTEPSPR) == USB_OTG_DOEPINT_OTEPSPR)
          {
            CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPSPR);
          }
#endif /* USB_OTG_DOEPINT_OTEPSPR */
        }
        epnum++;
        ep_intr >>= 1U;
      }
    }
    
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_IEPINT))
    {
      /* Read in the device interrupt bits */
      ep_intr = USB_ReadDevAllInEpInterrupt(hpcd->Instance);
      
      epnum = 0U;
      
      while ( ep_intr )
      {
        if (ep_intr & 0x1U) /* In ITR */
        {
          epint = USB_ReadDevInEPInterrupt(hpcd->Instance, epnum);

           if(( epint & USB_OTG_DIEPINT_XFRC) == USB_OTG_DIEPINT_XFRC)
          {
            fifoemptymsk = 0x1U << epnum;
            USBx_DEVICE->DIEPEMPMSK &= ~fifoemptymsk;
            
            CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_XFRC);
            
            if (hpcd->Init.dma_enable == 1U)
            {
              hpcd->IN_ep[epnum].xfer_buff += hpcd->IN_ep[epnum].maxpacket; 
            }
                                      
            HAL_PCD_DataInStageCallback(hpcd, epnum);

            if (hpcd->Init.dma_enable == 1U)
            {
              /* this is ZLP, so prepare EP0 for next setup */
              if((epnum == 0U) && (hpcd->IN_ep[epnum].xfer_len == 0U))
              {
                /* prepare to rx more setup packets */
                USB_EP0_OutStart(hpcd->Instance, 1U, (uint8_t *)hpcd->Setup);
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
        ep_intr >>= 1U;
      }
    }
    
    /* Handle Resume Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_WKUINT))
    {    
      /* Clear the Remote Wake-up Signaling */
      USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;

#ifdef USB_OTG_GLPMCFG_LPMEN
      if(hpcd->LPM_State == LPM_L1)
      {
        hpcd->LPM_State = LPM_L0;
        HAL_PCDEx_LPM_Callback(hpcd, PCD_LPM_L0_ACTIVE);
      }
      else
#endif /* USB_OTG_GLPMCFG_LPMEN */
      {
        HAL_PCD_ResumeCallback(hpcd);
      }
      
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_WKUINT);
    }
    
    /* Handle Suspend Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_USBSUSP))
    {
      if((USBx_DEVICE->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS)
      {
        
        HAL_PCD_SuspendCallback(hpcd);
      }
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_USBSUSP);
    }

#ifdef USB_OTG_GLPMCFG_LPMEN
    /* Handle LPM Interrupt */ 
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_LPMINT))
    {
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_LPMINT);      
      if( hpcd->LPM_State == LPM_L0)
      {
        hpcd->LPM_State = LPM_L1;
        hpcd->BESL = (hpcd->Instance->GLPMCFG & USB_OTG_GLPMCFG_BESL) >>2 ;
        HAL_PCDEx_LPM_Callback(hpcd, PCD_LPM_L1_ACTIVE);
      }
      else
      {
        HAL_PCD_SuspendCallback(hpcd);
      }
    }
#endif /* USB_OTG_GLPMCFG_LPMEN */

    /* Handle Reset Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_USBRST))
    {
      USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG; 
      USB_FlushTxFifo(hpcd->Instance , 0U);
      
      for (i = 0U; i < hpcd->Init.dev_endpoints; i++)
      {
        USBx_INEP(i)->DIEPINT = 0xFFU;
        USBx_OUTEP(i)->DOEPINT = 0xFFU;
      }
      USBx_DEVICE->DAINT = 0xFFFFFFFFU;
      USBx_DEVICE->DAINTMSK |= 0x10001U;
      
      if(hpcd->Init.use_dedicated_ep1)
      {
        USBx_DEVICE->DOUTEP1MSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM); 
        USBx_DEVICE->DINEP1MSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);  
      }
      else
      {
#ifdef USB_OTG_DOEPINT_OTEPSPR
        USBx_DEVICE->DOEPMSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM | USB_OTG_DOEPMSK_OTEPSPRM);
#else
        USBx_DEVICE->DOEPMSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM);
#endif /* USB_OTG_DOEPINT_OTEPSPR */
        USBx_DEVICE->DIEPMSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);
      }
      
      /* Set Default Address to 0 */
      USBx_DEVICE->DCFG &= ~USB_OTG_DCFG_DAD;
      
      /* setup EP0 to receive SETUP packets */
      USB_EP0_OutStart(hpcd->Instance, hpcd->Init.dma_enable, (uint8_t *)hpcd->Setup);
        
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_USBRST);
    }
    
    /* Handle Enumeration done Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_ENUMDNE))
    {
      USB_ActivateSetup(hpcd->Instance);
      hpcd->Instance->GUSBCFG &= ~USB_OTG_GUSBCFG_TRDT;
      
      if ( USB_GetDevSpeed(hpcd->Instance) == USB_OTG_SPEED_HIGH)
      {
        hpcd->Init.speed            = USB_OTG_SPEED_HIGH;
        hpcd->Init.ep0_mps          = USB_OTG_HS_MAX_PACKET_SIZE ;    
        hpcd->Instance->GUSBCFG |= (uint32_t)((USBD_HS_TRDT_VALUE << 10U) & USB_OTG_GUSBCFG_TRDT);
      }
      else
      {
        hpcd->Init.speed            = USB_OTG_SPEED_FULL;
        hpcd->Init.ep0_mps          = USB_OTG_FS_MAX_PACKET_SIZE ;  
        
        /* The USBTRD is configured according to the tables below, depending on AHB frequency 
        used by application. In the low AHB frequency range it is used to stretch enough the USB response 
        time to IN tokens, the USB turnaround time, so to compensate for the longer AHB read access 
        latency to the Data FIFO */
        
        /* Get hclk frequency value */
        hclk = HAL_RCC_GetHCLKFreq();
		
        if((hclk >= 14200000)&&(hclk < 15000000))
        {
          /* hclk Clock Range between 14.2-15 MHz */
          hpcd->Instance->GUSBCFG |= (uint32_t)((0xF << 10) & USB_OTG_GUSBCFG_TRDT);
        }
        
        else if((hclk >= 15000000)&&(hclk < 16000000))
        {
          /* hclk Clock Range between 15-16 MHz */
          hpcd->Instance->GUSBCFG |= (uint32_t)((0xE << 10) & USB_OTG_GUSBCFG_TRDT);
        }
        
        else if((hclk >= 16000000)&&(hclk < 17200000))
        {
          /* hclk Clock Range between 16-17.2 MHz */
          hpcd->Instance->GUSBCFG |= (uint32_t)((0xD << 10) & USB_OTG_GUSBCFG_TRDT);
        }
        
        else if((hclk >= 17200000)&&(hclk < 18500000))
        {
          /* hclk Clock Range between 17.2-18.5 MHz */
          hpcd->Instance->GUSBCFG |= (uint32_t)((0xC << 10) & USB_OTG_GUSBCFG_TRDT);
        }
        
        else if((hclk >= 18500000)&&(hclk < 20000000))
        {
          /* hclk Clock Range between 18.5-20 MHz */
          hpcd->Instance->GUSBCFG |= (uint32_t)((0xB << 10) & USB_OTG_GUSBCFG_TRDT);
        }
        
        else if((hclk >= 20000000)&&(hclk < 21800000))
        {
          /* hclk Clock Range between 20-21.8 MHz */
          hpcd->Instance->GUSBCFG |= (uint32_t)((0xA << 10) & USB_OTG_GUSBCFG_TRDT);
        }
        
        else if((hclk >= 21800000)&&(hclk < 24000000))
        {
          /* hclk Clock Range between 21.8-24 MHz */
          hpcd->Instance->GUSBCFG |= (uint32_t)((0x9 << 10) & USB_OTG_GUSBCFG_TRDT);
        }
        
        else if((hclk >= 24000000)&&(hclk < 27700000))
        {
          /* hclk Clock Range between 24-27.7 MHz */
          hpcd->Instance->GUSBCFG |= (uint32_t)((0x8 << 10) & USB_OTG_GUSBCFG_TRDT);
        }
        
        else if((hclk >= 27700000)&&(hclk < 32000000))
        {
          /* hclk Clock Range between 27.7-32 MHz */
          hpcd->Instance->GUSBCFG |= (uint32_t)((0x7 << 10) & USB_OTG_GUSBCFG_TRDT);
        }
        
        else /* if(hclk >= 32000000) */
        {
          /* hclk Clock Range between 32-180 MHz */
          hpcd->Instance->GUSBCFG |= (uint32_t)((0x6 << 10) & USB_OTG_GUSBCFG_TRDT);
        }  
      }
      
      HAL_PCD_ResetCallback(hpcd);
      
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_ENUMDNE);
    }

    /* Handle RxQLevel Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_RXFLVL))
    {
      USB_MASK_INTERRUPT(hpcd->Instance, USB_OTG_GINTSTS_RXFLVL);
      
      temp = USBx->GRXSTSP;
      
      ep = &hpcd->OUT_ep[temp & USB_OTG_GRXSTSP_EPNUM];
      
      if(((temp & USB_OTG_GRXSTSP_PKTSTS) >> 17U) ==  STS_DATA_UPDT)
      {
        if((temp & USB_OTG_GRXSTSP_BCNT) != 0U)
        {
          USB_ReadPacket(USBx, ep->xfer_buff, (temp & USB_OTG_GRXSTSP_BCNT) >> 4U);
          ep->xfer_buff += (temp & USB_OTG_GRXSTSP_BCNT) >> 4U;
          ep->xfer_count += (temp & USB_OTG_GRXSTSP_BCNT) >> 4U;
        }
      }
      else if (((temp & USB_OTG_GRXSTSP_PKTSTS) >> 17U) ==  STS_SETUP_UPDT)
      {
        USB_ReadPacket(USBx, (uint8_t *)hpcd->Setup, 8U);
        ep->xfer_count += (temp & USB_OTG_GRXSTSP_BCNT) >> 4U;
      }
      USB_UNMASK_INTERRUPT(hpcd->Instance, USB_OTG_GINTSTS_RXFLVL);
    }
    
    /* Handle SOF Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_SOF))
    {
      HAL_PCD_SOFCallback(hpcd);
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_SOF);
    }
    
    /* Handle Incomplete ISO IN Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_IISOIXFR))
    {
      HAL_PCD_ISOINIncompleteCallback(hpcd, epnum);
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_IISOIXFR);
    } 
    
    /* Handle Incomplete ISO OUT Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT))
    {
      HAL_PCD_ISOOUTIncompleteCallback(hpcd, epnum);
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT);
    } 
    
    /* Handle Connection event Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_SRQINT))
    {
      HAL_PCD_ConnectCallback(hpcd);
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_SRQINT);
    } 
    
    /* Handle Disconnection event Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_OTGINT))
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
  * @brief  Data OUT stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: endpoint number  
  * @retval None
  */
 __weak void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  UNUSED(epnum);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Data IN stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: endpoint number
  * @retval None
  */
 __weak void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  UNUSED(epnum);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataInStageCallback could be implemented in the user file
   */ 
}
/**
  * @brief  Setup stage callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_SetupStageCallback could be implemented in the user file
   */ 
}

/**
  * @brief  USB Start Of Frame callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_SOFCallback could be implemented in the user file
   */ 
}

/**
  * @brief  USB Reset callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_ResetCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Suspend event callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_SuspendCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Resume event callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_ResumeCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Incomplete ISO OUT callback.
  * @param  hpcd: PCD handle
  * @param  epnum: endpoint number
  * @retval None
  */
 __weak void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  UNUSED(epnum);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_ISOOUTIncompleteCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Incomplete ISO IN callback.
  * @param  hpcd: PCD handle
  * @param  epnum: endpoint number  
  * @retval None
  */
 __weak void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  UNUSED(epnum);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_ISOINIncompleteCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Connection event callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_ConnectCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Disconnection event callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
 __weak void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DisconnectCallback could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup PCD_Exported_Functions_Group3 Peripheral Control functions
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
  * @brief  Connect the USB device.
  * @param  hpcd: PCD handle
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
  * @brief  Disconnect the USB device.
  * @param  hpcd: PCD handle
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
  * @brief  Set the USB Device address. 
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
  * @brief  Open and configure an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  ep_mps: endpoint max packet size
  * @param  ep_type: endpoint type   
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type)
{
  HAL_StatusTypeDef  ret = HAL_OK;
  USB_OTG_EPTypeDef *ep;
  
  if ((ep_addr & 0x80U) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & 0x7FU];
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr & 0x7FU];
  }
  ep->num   = ep_addr & 0x7FU;
  
  ep->is_in = (0x80U & ep_addr) != 0U;
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
    ep->data_pid_start = 0U;
  }
  
  __HAL_LOCK(hpcd); 
  USB_ActivateEndpoint(hpcd->Instance , ep);
  __HAL_UNLOCK(hpcd);   
  return ret;
}


/**
  * @brief  Deactivate an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{  
  USB_OTG_EPTypeDef *ep;
  
  if ((ep_addr & 0x80U) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & 0x7FU];
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr & 0x7FU];
  }
  ep->num   = ep_addr & 0x7FU;
  
  ep->is_in = (0x80U & ep_addr) != 0U;
  
  __HAL_LOCK(hpcd); 
  USB_DeactivateEndpoint(hpcd->Instance , ep);
  __HAL_UNLOCK(hpcd);   
  return HAL_OK;
}


/**
  * @brief  Receive an amount of data.  
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  pBuf: pointer to the reception buffer   
  * @param  len: amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
  USB_OTG_EPTypeDef *ep;
  
  ep = &hpcd->OUT_ep[ep_addr & 0x7FU];
  
  /*setup and start the Xfer */
  ep->xfer_buff = pBuf;  
  ep->xfer_len = len;
  ep->xfer_count = 0U;
  ep->is_in = 0U;
  ep->num = ep_addr & 0x7FU;
  
  if (hpcd->Init.dma_enable == 1U)
  {
    ep->dma_addr = (uint32_t)pBuf;  
  }
  
  __HAL_LOCK(hpcd); 
  
  if ((ep_addr & 0x7FU) == 0U)
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
  * @brief  Get Received Data Size.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval Data Size
  */
uint16_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  return hpcd->OUT_ep[ep_addr & 0x7FU].xfer_count;
}
/**
  * @brief  Send an amount of data.  
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  pBuf: pointer to the transmission buffer   
  * @param  len: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
  USB_OTG_EPTypeDef *ep;
  
  ep = &hpcd->IN_ep[ep_addr & 0x7FU];
  
  /*setup and start the Xfer */
  ep->xfer_buff = pBuf;  
  ep->xfer_len = len;
  ep->xfer_count = 0U;
  ep->is_in = 1U;
  ep->num = ep_addr & 0x7FU;
  
  if (hpcd->Init.dma_enable == 1U)
  {
    ep->dma_addr = (uint32_t)pBuf;  
  }
  
  __HAL_LOCK(hpcd); 
  
  if ((ep_addr & 0x7FU) == 0U)
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
  * @brief  Set a STALL condition over an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  USB_OTG_EPTypeDef *ep;
  
  if ((0x80U & ep_addr) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & 0x7FU];
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr];
  }
  
  ep->is_stall = 1U;
  ep->num   = ep_addr & 0x7FU;
  ep->is_in = ((ep_addr & 0x80U) == 0x80U);
  
  
  __HAL_LOCK(hpcd); 
  USB_EPSetStall(hpcd->Instance , ep);
  if((ep_addr & 0x7FU) == 0U)
  {
    USB_EP0_OutStart(hpcd->Instance, hpcd->Init.dma_enable, (uint8_t *)hpcd->Setup);
  }
  __HAL_UNLOCK(hpcd); 
  
  return HAL_OK;
}

/**
  * @brief  Clear a STALL condition over in an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  USB_OTG_EPTypeDef *ep;
  
  if ((0x80U & ep_addr) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & 0x7FU];
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr];
  }
  
  ep->is_stall = 0U;
  ep->num   = ep_addr & 0x7FU;
  ep->is_in = ((ep_addr & 0x80U) == 0x80U);
  
  __HAL_LOCK(hpcd); 
  USB_EPClearStall(hpcd->Instance , ep);
  __HAL_UNLOCK(hpcd); 
    
  return HAL_OK;
}

/**
  * @brief  Flush an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  __HAL_LOCK(hpcd); 
  
  if ((ep_addr & 0x80U) == 0x80U)
  {
    USB_FlushTxFifo(hpcd->Instance, ep_addr & 0x7FU);
  }
  else
  {
    USB_FlushRxFifo(hpcd->Instance);
  }
  
  __HAL_UNLOCK(hpcd); 
    
  return HAL_OK;
}

/**
  * @brief  Activate remote wakeup signalling.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;  
    
  if((USBx_DEVICE->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS)
  {
    /* Activate Remote wakeup signaling */
    USBx_DEVICE->DCTL |= USB_OTG_DCTL_RWUSIG;
  }
  return HAL_OK;  
}

/**
  * @brief  De-activate remote wakeup signalling.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;  
  
  /* De-activate Remote wakeup signaling */
  USBx_DEVICE->DCTL &= ~(USB_OTG_DCTL_RWUSIG);
  return HAL_OK;  
}
/**
  * @}
  */
  
/** @defgroup PCD_Exported_Functions_Group4 Peripheral State functions 
 *  @brief   Peripheral State functions
 *
@verbatim  
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================  
    [..]
    This subsection permits to get in run-time the status of the peripheral 
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Return the PCD handle state.
  * @param  hpcd: PCD handle
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
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @addtogroup PCD_Private_Functions
  * @{
  */

/**
  * @brief  Check FIFO for the next packet to be loaded.
  * @param  hpcd: PCD handle
  * @param  epnum : endpoint number   
  * @retval HAL status
  */
static HAL_StatusTypeDef PCD_WriteEmptyTxFifo(PCD_HandleTypeDef *hpcd, uint32_t epnum)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;  
  USB_OTG_EPTypeDef *ep;
  int32_t len = 0U;
  uint32_t len32b;
  uint32_t fifoemptymsk = 0U;

  ep = &hpcd->IN_ep[epnum];
  len = ep->xfer_len - ep->xfer_count;
  
  if (len > ep->maxpacket)
  {
    len = ep->maxpacket;
  }
  
  
  len32b = (len + 3U) / 4U;
 
  while  ( (USBx_INEP(epnum)->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) > len32b &&
          ep->xfer_count < ep->xfer_len &&
            ep->xfer_len != 0U)
  {
    /* Write the FIFO */
    len = ep->xfer_len - ep->xfer_count;
    
    if (len > ep->maxpacket)
    {
      len = ep->maxpacket;
    }
    len32b = (len + 3U) / 4U;
    
    USB_WritePacket(USBx, ep->xfer_buff, epnum, len, hpcd->Init.dma_enable); 
    
    ep->xfer_buff  += len;
    ep->xfer_count += len;
  }
  
  if(len <= 0U)
  {
    fifoemptymsk = 0x1U << epnum;
    USBx_DEVICE->DIEPEMPMSK &= ~fifoemptymsk;
    
  }
  
  return HAL_OK;  
}

/**
  * @}
  */
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx ||
          STM32F401xC || STM32F401xE || STM32F411xE || STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Rx ||
          STM32F412Vx || STM32F412Cx */
#endif /* HAL_PCD_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
