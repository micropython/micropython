/**
  ******************************************************************************
  * @file    stm32n6xx_hal_hcd.c
  * @author  MCD Application Team
  * @brief   HCD HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the USB Peripheral Controller:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
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
                    ##### How to use this driver #####
  ==============================================================================
  [..]
    (#)Declare a HCD_HandleTypeDef handle structure, for example:
       HCD_HandleTypeDef  hhcd;

    (#)Fill parameters of Init structure in HCD handle

    (#)Call HAL_HCD_Init() API to initialize the HCD peripheral (Core, Host core, ...)

    (#)Initialize the HCD low level resources through the HAL_HCD_MspInit() API:
        (##) Enable the HCD/USB Low Level interface clock using the following macros
             (+++) __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
             (+++) __HAL_RCC_USB_OTG_HS_CLK_ENABLE(); (For High Speed Mode)
             (+++) __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE(); (For High Speed Mode)

        (##) Initialize the related GPIO clocks
        (##) Configure HCD pin-out
        (##) Configure HCD NVIC interrupt

    (#)Associate the Upper USB Host stack to the HAL HCD Driver:
        (##) hhcd.pData = phost;

    (#)Enable HCD transmission and reception:
        (##) HAL_HCD_Start();

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

#ifdef HAL_HCD_MODULE_ENABLED
#if defined (USB1_OTG_HS) || defined (USB2_OTG_HS)

/** @defgroup HCD HCD
  * @brief HCD HAL module driver
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup HCD_Private_Functions HCD Private Functions
  * @{
  */
static void HCD_HC_IN_IRQHandler(HCD_HandleTypeDef *hhcd, uint8_t chnum);
static void HCD_HC_OUT_IRQHandler(HCD_HandleTypeDef *hhcd, uint8_t chnum);
static void HCD_RXQLVL_IRQHandler(HCD_HandleTypeDef *hhcd);
static void HCD_Port_IRQHandler(HCD_HandleTypeDef *hhcd);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup HCD_Exported_Functions HCD Exported Functions
  * @{
  */

/** @defgroup HCD_Exported_Functions_Group1 Initialization and de-initialization functions
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
  * @brief  Initialize the host driver.
  * @param  hhcd HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_Init(HCD_HandleTypeDef *hhcd)
{
  /* Check the HCD handle allocation */
  if (hhcd == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_HCD_ALL_INSTANCE(hhcd->Instance));

  if (hhcd->State == HAL_HCD_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hhcd->Lock = HAL_UNLOCKED;

#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
    hhcd->SOFCallback = HAL_HCD_SOF_Callback;
    hhcd->ConnectCallback = HAL_HCD_Connect_Callback;
    hhcd->DisconnectCallback = HAL_HCD_Disconnect_Callback;
    hhcd->PortEnabledCallback = HAL_HCD_PortEnabled_Callback;
    hhcd->PortDisabledCallback = HAL_HCD_PortDisabled_Callback;
    hhcd->HC_NotifyURBChangeCallback = HAL_HCD_HC_NotifyURBChange_Callback;

    if (hhcd->MspInitCallback == NULL)
    {
      hhcd->MspInitCallback = HAL_HCD_MspInit;
    }

    /* Init the low level hardware */
    hhcd->MspInitCallback(hhcd);
#else
    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    HAL_HCD_MspInit(hhcd);
#endif /* (USE_HAL_HCD_REGISTER_CALLBACKS) */
  }

  hhcd->State = HAL_HCD_STATE_BUSY;

  /* Disable the Interrupts */
  __HAL_HCD_DISABLE(hhcd);

  /* Init the Core (common init.) */
  if (USB_CoreInit(hhcd->Instance, hhcd->Init) != HAL_OK)
  {
    hhcd->State = HAL_HCD_STATE_ERROR;
    return HAL_ERROR;
  }

  /* Force Host Mode */
  if (USB_SetCurrentMode(hhcd->Instance, USB_HOST_MODE) != HAL_OK)
  {
    hhcd->State = HAL_HCD_STATE_ERROR;
    return HAL_ERROR;
  }

  /* Init Host */
  if (USB_HostInit(hhcd->Instance, hhcd->Init) != HAL_OK)
  {
    hhcd->State = HAL_HCD_STATE_ERROR;
    return HAL_ERROR;
  }

  hhcd->State = HAL_HCD_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Initialize a host channel.
  * @param  hhcd HCD handle
  * @param  ch_num Channel number.
  *         This parameter can be a value from 1 to 15
  * @param  epnum Endpoint number.
  *          This parameter can be a value from 1 to 15
  * @param  dev_address Current device address
  *          This parameter can be a value from 0 to 255
  * @param  speed Current device speed.
  *          This parameter can be one of these values:
  *            HCD_DEVICE_SPEED_HIGH: High speed mode,
  *            HCD_DEVICE_SPEED_FULL: Full speed mode,
  *            HCD_DEVICE_SPEED_LOW: Low speed mode
  * @param  ep_type Endpoint Type.
  *          This parameter can be one of these values:
  *            EP_TYPE_CTRL: Control type,
  *            EP_TYPE_ISOC: Isochronous type,
  *            EP_TYPE_BULK: Bulk type,
  *            EP_TYPE_INTR: Interrupt type
  * @param  mps Max Packet Size.
  *          This parameter can be a value from 0 to32K
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_Init(HCD_HandleTypeDef *hhcd, uint8_t ch_num, uint8_t epnum,
                                  uint8_t dev_address, uint8_t speed, uint8_t ep_type, uint16_t mps)
{
  HAL_StatusTypeDef status;
  uint32_t HostCoreSpeed;
  uint32_t HCcharMps = mps;

  __HAL_LOCK(hhcd);
  hhcd->hc[ch_num].do_ping = 0U;
  hhcd->hc[ch_num].dev_addr = dev_address;
  hhcd->hc[ch_num].ch_num = ch_num;
  hhcd->hc[ch_num].ep_type = ep_type;
  hhcd->hc[ch_num].ep_num = epnum & 0x7FU;

  (void)HAL_HCD_HC_ClearHubInfo(hhcd, ch_num);

  if ((epnum & 0x80U) == 0x80U)
  {
    hhcd->hc[ch_num].ep_is_in = 1U;
  }
  else
  {
    hhcd->hc[ch_num].ep_is_in = 0U;
  }

  HostCoreSpeed = USB_GetHostSpeed(hhcd->Instance);

  if (ep_type == EP_TYPE_ISOC)
  {
    /* FS device plugged to HS HUB */
    if ((speed == HCD_DEVICE_SPEED_FULL) && (HostCoreSpeed == HPRT0_PRTSPD_HIGH_SPEED))
    {
      if (HCcharMps > ISO_SPLT_MPS)
      {
        /* ISO Max Packet Size for Split mode */
        HCcharMps = ISO_SPLT_MPS;
      }
    }
  }

  hhcd->hc[ch_num].speed = speed;
  hhcd->hc[ch_num].max_packet = (uint16_t)HCcharMps;

  status =  USB_HC_Init(hhcd->Instance, ch_num, epnum,
                        dev_address, speed, ep_type, (uint16_t)HCcharMps);

  __HAL_UNLOCK(hhcd);

  return status;
}

/**
  * @brief  Halt a host channel.
  * @param  hhcd HCD handle
  * @param  ch_num Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_Halt(HCD_HandleTypeDef *hhcd, uint8_t ch_num)
{
  HAL_StatusTypeDef status = HAL_OK;

  __HAL_LOCK(hhcd);
  (void)USB_HC_Halt(hhcd->Instance, ch_num);
  __HAL_UNLOCK(hhcd);

  return status;
}

/**
  * @brief  DeInitialize the host driver.
  * @param  hhcd HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_DeInit(HCD_HandleTypeDef *hhcd)
{
  /* Check the HCD handle allocation */
  if (hhcd == NULL)
  {
    return HAL_ERROR;
  }

  hhcd->State = HAL_HCD_STATE_BUSY;

#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
  if (hhcd->MspDeInitCallback == NULL)
  {
    hhcd->MspDeInitCallback = HAL_HCD_MspDeInit; /* Legacy weak MspDeInit  */
  }

  /* DeInit the low level hardware */
  hhcd->MspDeInitCallback(hhcd);
#else
  /* DeInit the low level hardware: CLOCK, NVIC.*/
  HAL_HCD_MspDeInit(hhcd);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */

  __HAL_HCD_DISABLE(hhcd);

  hhcd->State = HAL_HCD_STATE_RESET;

  return HAL_OK;
}

/**
  * @brief  Initialize the HCD MSP.
  * @param  hhcd HCD handle
  * @retval None
  */
__weak void  HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_HCD_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitialize the HCD MSP.
  * @param  hhcd HCD handle
  * @retval None
  */
__weak void  HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_HCD_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup HCD_Exported_Functions_Group2 Input and Output operation functions
  *  @brief   HCD IO operation functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
 [..] This subsection provides a set of functions allowing to manage the USB Host Data
    Transfer

@endverbatim
  * @{
  */

/**
  * @brief  Submit a new URB for processing.
  * @param  hhcd HCD handle
  * @param  ch_num Channel number.
  *         This parameter can be a value from 1 to 15
  * @param  direction Channel number.
  *          This parameter can be one of these values:
  *           0 : Output / 1 : Input
  * @param  ep_type Endpoint Type.
  *          This parameter can be one of these values:
  *            EP_TYPE_CTRL: Control type/
  *            EP_TYPE_ISOC: Isochronous type/
  *            EP_TYPE_BULK: Bulk type/
  *            EP_TYPE_INTR: Interrupt type/
  * @param  token Endpoint Type.
  *          This parameter can be one of these values:
  *            0: HC_PID_SETUP / 1: HC_PID_DATA1
  * @param  pbuff pointer to URB data
  * @param  length Length of URB data
  * @param  do_ping activate do ping protocol (for high speed only).
  *          This parameter can be one of these values:
  *           0 : do ping inactive / 1 : do ping active
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_SubmitRequest(HCD_HandleTypeDef *hhcd,
                                           uint8_t ch_num,
                                           uint8_t direction,
                                           uint8_t ep_type,
                                           uint8_t token,
                                           uint8_t *pbuff,
                                           uint16_t length,
                                           uint8_t do_ping)
{
  hhcd->hc[ch_num].ep_is_in = direction;
  hhcd->hc[ch_num].ep_type  = ep_type;

  if (token == 0U)
  {
    hhcd->hc[ch_num].data_pid = HC_PID_SETUP;
    hhcd->hc[ch_num].do_ping = do_ping;
  }
  else
  {
    hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
  }

  /* Manage Data Toggle */
  switch (ep_type)
  {
    case EP_TYPE_CTRL:
      if (token == 1U) /* send data */
      {
        if (direction == 0U)
        {
          if (length == 0U)
          {
            /* For Status OUT stage, Length == 0U, Status Out PID = 1 */
            hhcd->hc[ch_num].toggle_out = 1U;
          }

          /* Set the Data Toggle bit as per the Flag */
          if (hhcd->hc[ch_num].toggle_out == 0U)
          {
            /* Put the PID 0 */
            hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
          }
          else
          {
            /* Put the PID 1 */
            hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
          }
        }
        else
        {
          if (hhcd->hc[ch_num].do_ssplit == 1U)
          {
            if (hhcd->hc[ch_num].toggle_in == 0U)
            {
              hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
            }
            else
            {
              hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
            }
          }
        }
      }
      break;

    case EP_TYPE_BULK:
      if (direction == 0U)
      {
        /* Set the Data Toggle bit as per the Flag */
        if (hhcd->hc[ch_num].toggle_out == 0U)
        {
          /* Put the PID 0 */
          hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
        }
        else
        {
          /* Put the PID 1 */
          hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
        }
      }
      else
      {
        if (hhcd->hc[ch_num].toggle_in == 0U)
        {
          hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
        }
        else
        {
          hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
        }
      }

      break;
    case EP_TYPE_INTR:
      if (direction == 0U)
      {
        /* Set the Data Toggle bit as per the Flag */
        if (hhcd->hc[ch_num].toggle_out == 0U)
        {
          /* Put the PID 0 */
          hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
        }
        else
        {
          /* Put the PID 1 */
          hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
        }
      }
      else
      {
        if (hhcd->hc[ch_num].toggle_in == 0U)
        {
          hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
        }
        else
        {
          hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
        }
      }
      break;

    case EP_TYPE_ISOC:
      hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
      break;

    default:
      break;
  }

  hhcd->hc[ch_num].xfer_buff = pbuff;
  hhcd->hc[ch_num].xfer_len  = length;
  hhcd->hc[ch_num].urb_state = URB_IDLE;
  hhcd->hc[ch_num].xfer_count = 0U;
  hhcd->hc[ch_num].ch_num = ch_num;
  hhcd->hc[ch_num].state = HC_IDLE;

  return USB_HC_StartXfer(hhcd->Instance, &hhcd->hc[ch_num], (uint8_t)hhcd->Init.dma_enable);
}

/**
  * @brief  Handle HCD interrupt request.
  * @param  hhcd HCD handle
  * @retval None
  */
void HAL_HCD_IRQHandler(HCD_HandleTypeDef *hhcd)
{
  USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;
  uint32_t USBx_BASE = (uint32_t)USBx;
  uint32_t i;
  uint32_t interrupt;

  /* Ensure that we are in device mode */
  if (USB_GetMode(hhcd->Instance) == USB_OTG_MODE_HOST)
  {
    /* Avoid spurious interrupt */
    if (__HAL_HCD_IS_INVALID_INTERRUPT(hhcd))
    {
      return;
    }

    if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT))
    {
      /* Incorrect mode, acknowledge the interrupt */
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT);
    }

    if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_IISOIXFR))
    {
      /* Incorrect mode, acknowledge the interrupt */
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_IISOIXFR);
    }

    if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_PTXFE))
    {
      /* Incorrect mode, acknowledge the interrupt */
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_PTXFE);
    }

    if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_MMIS))
    {
      /* Incorrect mode, acknowledge the interrupt */
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_MMIS);
    }

    /* Handle Host Disconnect Interrupts */
    if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_DISCINT))
    {
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_DISCINT);

      if ((USBx_HPRT0 & USB_OTG_HPRT_PCSTS) == 0U)
      {
        /* Flush USB Fifo */
        (void)USB_FlushTxFifo(USBx, 0x10U);
        (void)USB_FlushRxFifo(USBx);


        /* Handle Host Port Disconnect Interrupt */
#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
        hhcd->DisconnectCallback(hhcd);
#else
        HAL_HCD_Disconnect_Callback(hhcd);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */
      }
    }

    /* Handle Host Port Interrupts */
    if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_HPRTINT))
    {
      HCD_Port_IRQHandler(hhcd);
    }

    /* Handle Host SOF Interrupt */
    if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_SOF))
    {
#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
      hhcd->SOFCallback(hhcd);
#else
      HAL_HCD_SOF_Callback(hhcd);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */

      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_SOF);
    }

    /* Handle Host channel Interrupt */
    if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_HCINT))
    {
      interrupt = USB_HC_ReadInterrupt(hhcd->Instance);
      for (i = 0U; i < hhcd->Init.Host_channels; i++)
      {
        if ((interrupt & (1UL << (i & 0xFU))) != 0U)
        {
          if ((USBx_HC(i)->HCCHAR & USB_OTG_HCCHAR_EPDIR) == USB_OTG_HCCHAR_EPDIR)
          {
            HCD_HC_IN_IRQHandler(hhcd, (uint8_t)i);
          }
          else
          {
            HCD_HC_OUT_IRQHandler(hhcd, (uint8_t)i);
          }
        }
      }
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_HCINT);
    }

    /* Handle Rx Queue Level Interrupts */
    if ((__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_RXFLVL)) != 0U)
    {
      USB_MASK_INTERRUPT(hhcd->Instance, USB_OTG_GINTSTS_RXFLVL);

      HCD_RXQLVL_IRQHandler(hhcd);

      USB_UNMASK_INTERRUPT(hhcd->Instance, USB_OTG_GINTSTS_RXFLVL);
    }
  }
}


/**
  * @brief  SOF callback.
  * @param  hhcd HCD handle
  * @retval None
  */
__weak void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_HCD_SOF_Callback could be implemented in the user file
   */
}

/**
  * @brief Connection Event callback.
  * @param  hhcd HCD handle
  * @retval None
  */
__weak void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_HCD_Connect_Callback could be implemented in the user file
   */
}

/**
  * @brief  Disconnection Event callback.
  * @param  hhcd HCD handle
  * @retval None
  */
__weak void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_HCD_Disconnect_Callback could be implemented in the user file
   */
}

/**
  * @brief  Port Enabled  Event callback.
  * @param  hhcd HCD handle
  * @retval None
  */
__weak void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef *hhcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_HCD_Disconnect_Callback could be implemented in the user file
   */
}

/**
  * @brief  Port Disabled  Event callback.
  * @param  hhcd HCD handle
  * @retval None
  */
__weak void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef *hhcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_HCD_Disconnect_Callback could be implemented in the user file
   */
}

/**
  * @brief  Notify URB state change callback.
  * @param  hhcd HCD handle
  * @param  chnum Channel number.
  *         This parameter can be a value from 1 to 15
  * @param  urb_state:
  *          This parameter can be one of these values:
  *            URB_IDLE/
  *            URB_DONE/
  *            URB_NOTREADY/
  *            URB_NYET/
  *            URB_ERROR/
  *            URB_STALL/
  * @retval None
  */
__weak void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);
  UNUSED(chnum);
  UNUSED(urb_state);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_HCD_HC_NotifyURBChange_Callback could be implemented in the user file
   */
}

#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
/**
  * @brief  Register a User USB HCD Callback
  *         To be used instead of the weak predefined callback
  * @param  hhcd USB HCD handle
  * @param  CallbackID ID of the callback to be registered
  *         This parameter can be one of the following values:
  *          @arg @ref HAL_HCD_SOF_CB_ID USB HCD SOF callback ID
  *          @arg @ref HAL_HCD_CONNECT_CB_ID USB HCD Connect callback ID
  *          @arg @ref HAL_HCD_DISCONNECT_CB_ID OTG HCD Disconnect callback ID
  *          @arg @ref HAL_HCD_PORT_ENABLED_CB_ID USB HCD Port Enable callback ID
  *          @arg @ref HAL_HCD_PORT_DISABLED_CB_ID USB HCD Port Disable callback ID
  *          @arg @ref HAL_HCD_MSPINIT_CB_ID MspDeInit callback ID
  *          @arg @ref HAL_HCD_MSPDEINIT_CB_ID MspDeInit callback ID
  * @param  pCallback pointer to the Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_RegisterCallback(HCD_HandleTypeDef *hhcd,
                                           HAL_HCD_CallbackIDTypeDef CallbackID,
                                           pHCD_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hhcd->ErrorCode |= HAL_HCD_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hhcd);

  if (hhcd->State == HAL_HCD_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_HCD_SOF_CB_ID :
        hhcd->SOFCallback = pCallback;
        break;

      case HAL_HCD_CONNECT_CB_ID :
        hhcd->ConnectCallback = pCallback;
        break;

      case HAL_HCD_DISCONNECT_CB_ID :
        hhcd->DisconnectCallback = pCallback;
        break;

      case HAL_HCD_PORT_ENABLED_CB_ID :
        hhcd->PortEnabledCallback = pCallback;
        break;

      case HAL_HCD_PORT_DISABLED_CB_ID :
        hhcd->PortDisabledCallback = pCallback;
        break;

      case HAL_HCD_MSPINIT_CB_ID :
        hhcd->MspInitCallback = pCallback;
        break;

      case HAL_HCD_MSPDEINIT_CB_ID :
        hhcd->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hhcd->ErrorCode |= HAL_HCD_ERROR_INVALID_CALLBACK;
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hhcd->State == HAL_HCD_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_HCD_MSPINIT_CB_ID :
        hhcd->MspInitCallback = pCallback;
        break;

      case HAL_HCD_MSPDEINIT_CB_ID :
        hhcd->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hhcd->ErrorCode |= HAL_HCD_ERROR_INVALID_CALLBACK;
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hhcd->ErrorCode |= HAL_HCD_ERROR_INVALID_CALLBACK;
    /* Return error status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hhcd);
  return status;
}

/**
  * @brief  Unregister an USB HCD Callback
  *         USB HCD callback is redirected to the weak predefined callback
  * @param  hhcd USB HCD handle
  * @param  CallbackID ID of the callback to be unregistered
  *         This parameter can be one of the following values:
  *          @arg @ref HAL_HCD_SOF_CB_ID USB HCD SOF callback ID
  *          @arg @ref HAL_HCD_CONNECT_CB_ID USB HCD Connect callback ID
  *          @arg @ref HAL_HCD_DISCONNECT_CB_ID OTG HCD Disconnect callback ID
  *          @arg @ref HAL_HCD_PORT_ENABLED_CB_ID USB HCD Port Enabled callback ID
  *          @arg @ref HAL_HCD_PORT_DISABLED_CB_ID USB HCD Port Disabled callback ID
  *          @arg @ref HAL_HCD_MSPINIT_CB_ID MspDeInit callback ID
  *          @arg @ref HAL_HCD_MSPDEINIT_CB_ID MspDeInit callback ID
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_UnRegisterCallback(HCD_HandleTypeDef *hhcd, HAL_HCD_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Process locked */
  __HAL_LOCK(hhcd);

  /* Setup Legacy weak Callbacks  */
  if (hhcd->State == HAL_HCD_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_HCD_SOF_CB_ID :
        hhcd->SOFCallback = HAL_HCD_SOF_Callback;
        break;

      case HAL_HCD_CONNECT_CB_ID :
        hhcd->ConnectCallback = HAL_HCD_Connect_Callback;
        break;

      case HAL_HCD_DISCONNECT_CB_ID :
        hhcd->DisconnectCallback = HAL_HCD_Disconnect_Callback;
        break;

      case HAL_HCD_PORT_ENABLED_CB_ID :
        hhcd->PortEnabledCallback = HAL_HCD_PortEnabled_Callback;
        break;

      case HAL_HCD_PORT_DISABLED_CB_ID :
        hhcd->PortDisabledCallback = HAL_HCD_PortDisabled_Callback;
        break;

      case HAL_HCD_MSPINIT_CB_ID :
        hhcd->MspInitCallback = HAL_HCD_MspInit;
        break;

      case HAL_HCD_MSPDEINIT_CB_ID :
        hhcd->MspDeInitCallback = HAL_HCD_MspDeInit;
        break;

      default :
        /* Update the error code */
        hhcd->ErrorCode |= HAL_HCD_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hhcd->State == HAL_HCD_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_HCD_MSPINIT_CB_ID :
        hhcd->MspInitCallback = HAL_HCD_MspInit;
        break;

      case HAL_HCD_MSPDEINIT_CB_ID :
        hhcd->MspDeInitCallback = HAL_HCD_MspDeInit;
        break;

      default :
        /* Update the error code */
        hhcd->ErrorCode |= HAL_HCD_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hhcd->ErrorCode |= HAL_HCD_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hhcd);
  return status;
}

/**
  * @brief  Register USB HCD Host Channel Notify URB Change Callback
  *         To be used instead of the weak HAL_HCD_HC_NotifyURBChange_Callback() predefined callback
  * @param  hhcd HCD handle
  * @param  pCallback pointer to the USB HCD Host Channel Notify URB Change Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_RegisterHC_NotifyURBChangeCallback(HCD_HandleTypeDef *hhcd,
                                                             pHCD_HC_NotifyURBChangeCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hhcd->ErrorCode |= HAL_HCD_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hhcd);

  if (hhcd->State == HAL_HCD_STATE_READY)
  {
    hhcd->HC_NotifyURBChangeCallback = pCallback;
  }
  else
  {
    /* Update the error code */
    hhcd->ErrorCode |= HAL_HCD_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hhcd);

  return status;
}

/**
  * @brief  Unregister the USB HCD Host Channel Notify URB Change Callback
  *         USB HCD Host Channel Notify URB Change Callback is redirected
  *         to the weak HAL_HCD_HC_NotifyURBChange_Callback() predefined callback
  * @param  hhcd HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_UnRegisterHC_NotifyURBChangeCallback(HCD_HandleTypeDef *hhcd)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Process locked */
  __HAL_LOCK(hhcd);

  if (hhcd->State == HAL_HCD_STATE_READY)
  {
    hhcd->HC_NotifyURBChangeCallback = HAL_HCD_HC_NotifyURBChange_Callback; /* Legacy weak DataOutStageCallback  */
  }
  else
  {
    /* Update the error code */
    hhcd->ErrorCode |= HAL_HCD_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hhcd);

  return status;
}
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup HCD_Exported_Functions_Group3 Peripheral Control functions
  *  @brief   Management functions
  *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the HCD data
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Start the host driver.
  * @param  hhcd HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_Start(HCD_HandleTypeDef *hhcd)
{
  __HAL_LOCK(hhcd);
  /* Enable port power */
  (void)USB_DriveVbus(hhcd->Instance, 1U);

  /* Enable global interrupt */
  __HAL_HCD_ENABLE(hhcd);
  __HAL_UNLOCK(hhcd);

  return HAL_OK;
}

/**
  * @brief  Stop the host driver.
  * @param  hhcd HCD handle
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_HCD_Stop(HCD_HandleTypeDef *hhcd)
{
  __HAL_LOCK(hhcd);
  (void)USB_StopHost(hhcd->Instance);
  __HAL_UNLOCK(hhcd);

  return HAL_OK;
}

/**
  * @brief  Reset the host port.
  * @param  hhcd HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_ResetPort(HCD_HandleTypeDef *hhcd)
{
  return (USB_ResetPort(hhcd->Instance));
}

/**
  * @}
  */

/** @defgroup HCD_Exported_Functions_Group4 Peripheral State functions
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
  * @brief  Return the HCD handle state.
  * @param  hhcd HCD handle
  * @retval HAL state
  */
HCD_StateTypeDef HAL_HCD_GetState(HCD_HandleTypeDef const *hhcd)
{
  return hhcd->State;
}

/**
  * @brief  Return  URB state for a channel.
  * @param  hhcd HCD handle
  * @param  chnum Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval URB state.
  *          This parameter can be one of these values:
  *            URB_IDLE/
  *            URB_DONE/
  *            URB_NOTREADY/
  *            URB_NYET/
  *            URB_ERROR/
  *            URB_STALL
  */
HCD_URBStateTypeDef HAL_HCD_HC_GetURBState(HCD_HandleTypeDef const *hhcd, uint8_t chnum)
{
  return hhcd->hc[chnum].urb_state;
}


/**
  * @brief  Return the last host transfer size.
  * @param  hhcd HCD handle
  * @param  chnum Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval last transfer size in byte
  */
uint32_t HAL_HCD_HC_GetXferCount(HCD_HandleTypeDef const *hhcd, uint8_t chnum)
{
  return hhcd->hc[chnum].xfer_count;
}

/**
  * @brief  Return the Host Channel state.
  * @param  hhcd HCD handle
  * @param  chnum Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval Host channel state
  *          This parameter can be one of these values:
  *            HC_IDLE/
  *            HC_XFRC/
  *            HC_HALTED/
  *            HC_NYET/
  *            HC_NAK/
  *            HC_STALL/
  *            HC_XACTERR/
  *            HC_BBLERR/
  *            HC_DATATGLERR
  */
HCD_HCStateTypeDef  HAL_HCD_HC_GetState(HCD_HandleTypeDef const *hhcd, uint8_t chnum)
{
  return hhcd->hc[chnum].state;
}

/**
  * @brief  Return the current Host frame number.
  * @param  hhcd HCD handle
  * @retval Current Host frame number
  */
uint32_t HAL_HCD_GetCurrentFrame(HCD_HandleTypeDef *hhcd)
{
  return (USB_GetCurrentFrame(hhcd->Instance));
}

/**
  * @brief  Return the Host enumeration speed.
  * @param  hhcd HCD handle
  * @retval Enumeration speed
  */
uint32_t HAL_HCD_GetCurrentSpeed(HCD_HandleTypeDef *hhcd)
{
  return (USB_GetHostSpeed(hhcd->Instance));
}

/**
  * @brief  Set host channel Hub information.
  * @param  hhcd HCD handle
  * @param  ch_num Channel number.
  *         This parameter can be a value from 1 to 15
  * @param  addr Hub address
  * @param  PortNbr Hub port number
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_SetHubInfo(HCD_HandleTypeDef *hhcd, uint8_t ch_num,
                                        uint8_t addr, uint8_t PortNbr)
{
  uint32_t HostCoreSpeed = USB_GetHostSpeed(hhcd->Instance);

  /* LS/FS device plugged to HS HUB */
  if ((hhcd->hc[ch_num].speed != HCD_DEVICE_SPEED_HIGH) && (HostCoreSpeed == HPRT0_PRTSPD_HIGH_SPEED))
  {
    hhcd->hc[ch_num].do_ssplit = 1U;

    if ((hhcd->hc[ch_num].ep_type == EP_TYPE_CTRL) && (hhcd->hc[ch_num].ep_is_in != 0U))
    {
      hhcd->hc[ch_num].toggle_in = 1U;
    }
  }

  hhcd->hc[ch_num].hub_addr = addr;
  hhcd->hc[ch_num].hub_port_nbr = PortNbr;

  return HAL_OK;
}


/**
  * @brief  Clear host channel hub information.
  * @param  hhcd HCD handle
  * @param  ch_num Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_ClearHubInfo(HCD_HandleTypeDef *hhcd, uint8_t ch_num)
{
  hhcd->hc[ch_num].do_ssplit = 0U;
  hhcd->hc[ch_num].do_csplit = 0U;
  hhcd->hc[ch_num].hub_addr = 0U;
  hhcd->hc[ch_num].hub_port_nbr = 0U;

  return HAL_OK;
}
/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup HCD_Private_Functions
  * @{
  */
/**
  * @brief  Handle Host Channel IN interrupt requests.
  * @param  hhcd HCD handle
  * @param  chnum Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval none
  */
static void HCD_HC_IN_IRQHandler(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  const USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;
  uint32_t USBx_BASE = (uint32_t)USBx;
  uint32_t tmpreg;

  if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_AHBERR))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_AHBERR);
    hhcd->hc[chnum].state = HC_XACTERR;
    (void)USB_HC_Halt(hhcd->Instance, chnum);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_BBERR))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_BBERR);
    hhcd->hc[chnum].state = HC_BBLERR;
    (void)USB_HC_Halt(hhcd->Instance, chnum);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_STALL))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_STALL);
    hhcd->hc[chnum].state = HC_STALL;
    (void)USB_HC_Halt(hhcd->Instance, chnum);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_DTERR))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_DTERR);
    hhcd->hc[chnum].state = HC_DATATGLERR;
    (void)USB_HC_Halt(hhcd->Instance, chnum);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_TXERR))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_TXERR);
    hhcd->hc[chnum].state = HC_XACTERR;
    (void)USB_HC_Halt(hhcd->Instance, chnum);
  }
  else
  {
    /* ... */
  }

  if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_FRMOR))
  {
    (void)USB_HC_Halt(hhcd->Instance, chnum);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_FRMOR);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_XFRC))
  {
    /* Clear any pending ACK IT */
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_ACK);

    if (hhcd->hc[chnum].do_csplit == 1U)
    {
      hhcd->hc[chnum].do_csplit = 0U;
      __HAL_HCD_CLEAR_HC_CSPLT(chnum);
    }

    if (hhcd->Init.dma_enable != 0U)
    {
      hhcd->hc[chnum].xfer_count = hhcd->hc[chnum].XferSize - (USBx_HC(chnum)->HCTSIZ & USB_OTG_HCTSIZ_XFRSIZ);
    }

    hhcd->hc[chnum].state = HC_XFRC;
    hhcd->hc[chnum].ErrCnt = 0U;
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_XFRC);

    if ((hhcd->hc[chnum].ep_type == EP_TYPE_CTRL) ||
        (hhcd->hc[chnum].ep_type == EP_TYPE_BULK))
    {
      (void)USB_HC_Halt(hhcd->Instance, chnum);
      __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);
    }
    else if ((hhcd->hc[chnum].ep_type == EP_TYPE_INTR) ||
             (hhcd->hc[chnum].ep_type == EP_TYPE_ISOC))
    {
      USBx_HC(chnum)->HCCHAR |= USB_OTG_HCCHAR_ODDFRM;
      hhcd->hc[chnum].urb_state = URB_DONE;

#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
      hhcd->HC_NotifyURBChangeCallback(hhcd, chnum, hhcd->hc[chnum].urb_state);
#else
      HAL_HCD_HC_NotifyURBChange_Callback(hhcd, chnum, hhcd->hc[chnum].urb_state);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */
    }
    else
    {
      /* ... */
    }

    if (hhcd->Init.dma_enable == 1U)
    {
      if ((((hhcd->hc[chnum].xfer_count + hhcd->hc[chnum].max_packet - 1U) / hhcd->hc[chnum].max_packet) & 1U) != 0U)
      {
        hhcd->hc[chnum].toggle_in ^= 1U;
      }
    }
    else
    {
      hhcd->hc[chnum].toggle_in ^= 1U;
    }
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_ACK))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_ACK);

    if (hhcd->hc[chnum].do_ssplit == 1U)
    {
      hhcd->hc[chnum].do_csplit = 1U;
      hhcd->hc[chnum].state = HC_ACK;

      (void)USB_HC_Halt(hhcd->Instance, chnum);
    }
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_CHH))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_CHH);

    if (hhcd->hc[chnum].state == HC_XFRC)
    {
      hhcd->hc[chnum].state = HC_HALTED;
      hhcd->hc[chnum].urb_state = URB_DONE;
    }
    else if (hhcd->hc[chnum].state == HC_STALL)
    {
      hhcd->hc[chnum].state = HC_HALTED;
      hhcd->hc[chnum].urb_state = URB_STALL;
    }
    else if ((hhcd->hc[chnum].state == HC_XACTERR) ||
             (hhcd->hc[chnum].state == HC_DATATGLERR))
    {
      hhcd->hc[chnum].state = HC_HALTED;
      hhcd->hc[chnum].ErrCnt++;
      if (hhcd->hc[chnum].ErrCnt > 2U)
      {
        hhcd->hc[chnum].ErrCnt = 0U;

        if (hhcd->hc[chnum].do_ssplit == 1U)
        {
          hhcd->hc[chnum].do_csplit = 0U;
          hhcd->hc[chnum].ep_ss_schedule = 0U;
          __HAL_HCD_CLEAR_HC_CSPLT(chnum);
        }

        hhcd->hc[chnum].urb_state = URB_ERROR;
      }
      else
      {
        hhcd->hc[chnum].urb_state = URB_NOTREADY;

        if ((hhcd->hc[chnum].ep_type == EP_TYPE_CTRL) ||
            (hhcd->hc[chnum].ep_type == EP_TYPE_BULK))
        {
          /* re-activate the channel */
          tmpreg = USBx_HC(chnum)->HCCHAR;
          tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
          tmpreg |= USB_OTG_HCCHAR_CHENA;
          USBx_HC(chnum)->HCCHAR = tmpreg;
        }
      }
    }
    else if (hhcd->hc[chnum].state == HC_NYET)
    {
      hhcd->hc[chnum].state = HC_HALTED;

      if (hhcd->hc[chnum].do_csplit == 1U)
      {
        if (hhcd->hc[chnum].ep_type == EP_TYPE_INTR)
        {
          hhcd->hc[chnum].NyetErrCnt++;
          if (hhcd->hc[chnum].NyetErrCnt > 2U)
          {
            hhcd->hc[chnum].NyetErrCnt = 0U;
            hhcd->hc[chnum].do_csplit = 0U;

            if (hhcd->hc[chnum].ErrCnt < 3U)
            {
              hhcd->hc[chnum].ep_ss_schedule = 1U;
            }
            __HAL_HCD_CLEAR_HC_CSPLT(chnum);
            hhcd->hc[chnum].urb_state = URB_ERROR;
          }
          else
          {
            hhcd->hc[chnum].urb_state = URB_NOTREADY;
          }
        }
        else
        {
          hhcd->hc[chnum].urb_state = URB_NOTREADY;
        }

        if ((hhcd->hc[chnum].ep_type == EP_TYPE_CTRL) ||
            (hhcd->hc[chnum].ep_type == EP_TYPE_BULK))
        {
          /* re-activate the channel */
          tmpreg = USBx_HC(chnum)->HCCHAR;
          tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
          tmpreg |= USB_OTG_HCCHAR_CHENA;
          USBx_HC(chnum)->HCCHAR = tmpreg;
        }
      }
    }
    else if (hhcd->hc[chnum].state == HC_ACK)
    {
      hhcd->hc[chnum].state = HC_HALTED;

      if (hhcd->hc[chnum].do_csplit == 1U)
      {
        hhcd->hc[chnum].urb_state = URB_NOTREADY;

        /* Set Complete split and re-activate the channel */
        USBx_HC(chnum)->HCSPLT |= USB_OTG_HCSPLT_COMPLSPLT;
        USBx_HC(chnum)->HCINTMSK |= USB_OTG_HCINTMSK_NYET;
        USBx_HC(chnum)->HCINTMSK &= ~USB_OTG_HCINT_ACK;

        if ((hhcd->hc[chnum].ep_type == EP_TYPE_CTRL) ||
            (hhcd->hc[chnum].ep_type == EP_TYPE_BULK))
        {
          /* re-activate the channel */
          tmpreg = USBx_HC(chnum)->HCCHAR;
          tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
          tmpreg |= USB_OTG_HCCHAR_CHENA;
          USBx_HC(chnum)->HCCHAR = tmpreg;
        }
      }
    }
    else if (hhcd->hc[chnum].state == HC_NAK)
    {
      hhcd->hc[chnum].state = HC_HALTED;
      hhcd->hc[chnum].urb_state = URB_NOTREADY;

      if ((hhcd->hc[chnum].ep_type == EP_TYPE_CTRL) ||
          (hhcd->hc[chnum].ep_type == EP_TYPE_BULK))
      {
        /* re-activate the channel */
        tmpreg = USBx_HC(chnum)->HCCHAR;
        tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
        tmpreg |= USB_OTG_HCCHAR_CHENA;
        USBx_HC(chnum)->HCCHAR = tmpreg;
      }
    }
    else if (hhcd->hc[chnum].state == HC_BBLERR)
    {
      hhcd->hc[chnum].state = HC_HALTED;
      hhcd->hc[chnum].ErrCnt++;
      hhcd->hc[chnum].urb_state = URB_ERROR;
    }
    else
    {
      if (hhcd->hc[chnum].state == HC_HALTED)
      {
        return;
      }
    }

#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
    hhcd->HC_NotifyURBChangeCallback(hhcd, chnum, hhcd->hc[chnum].urb_state);
#else
    HAL_HCD_HC_NotifyURBChange_Callback(hhcd, chnum, hhcd->hc[chnum].urb_state);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_NYET))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NYET);
    hhcd->hc[chnum].state = HC_NYET;

    if (hhcd->hc[chnum].do_ssplit == 0U)
    {
      hhcd->hc[chnum].ErrCnt = 0U;
    }

    (void)USB_HC_Halt(hhcd->Instance, chnum);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_NAK))
  {
    if (hhcd->hc[chnum].ep_type == EP_TYPE_INTR)
    {
      hhcd->hc[chnum].ErrCnt = 0U;
      hhcd->hc[chnum].state = HC_NAK;
      (void)USB_HC_Halt(hhcd->Instance, chnum);
    }
    else if ((hhcd->hc[chnum].ep_type == EP_TYPE_CTRL) ||
             (hhcd->hc[chnum].ep_type == EP_TYPE_BULK))
    {
      hhcd->hc[chnum].ErrCnt = 0U;

      if ((hhcd->Init.dma_enable == 0U) || (hhcd->hc[chnum].do_csplit == 1U))
      {
        hhcd->hc[chnum].state = HC_NAK;
        (void)USB_HC_Halt(hhcd->Instance, chnum);
      }
    }
    else
    {
      /* ... */
    }

    if (hhcd->hc[chnum].do_csplit == 1U)
    {
      hhcd->hc[chnum].do_csplit = 0U;
      __HAL_HCD_CLEAR_HC_CSPLT(chnum);
      __HAL_HCD_UNMASK_ACK_HC_INT(chnum);
    }

    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);
  }
  else
  {
    /* ... */
  }
}

/**
  * @brief  Handle Host Channel OUT interrupt requests.
  * @param  hhcd HCD handle
  * @param  chnum Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval none
  */
static void HCD_HC_OUT_IRQHandler(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  const USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;
  uint32_t USBx_BASE = (uint32_t)USBx;
  uint32_t tmpreg;
  uint32_t num_packets;

  if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_AHBERR))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_AHBERR);
    hhcd->hc[chnum].state = HC_XACTERR;
    (void)USB_HC_Halt(hhcd->Instance, chnum);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_ACK))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_ACK);

    if (hhcd->hc[chnum].do_ping == 1U)
    {
      hhcd->hc[chnum].do_ping = 0U;
      hhcd->hc[chnum].urb_state = URB_NOTREADY;
      hhcd->hc[chnum].state = HC_ACK;
      (void)USB_HC_Halt(hhcd->Instance, chnum);
    }

    if ((hhcd->hc[chnum].do_ssplit == 1U) && (hhcd->hc[chnum].do_csplit == 0U))
    {
      if (hhcd->hc[chnum].ep_type != EP_TYPE_ISOC)
      {
        hhcd->hc[chnum].do_csplit = 1U;
      }

      hhcd->hc[chnum].state = HC_ACK;
      (void)USB_HC_Halt(hhcd->Instance, chnum);

      /* reset error_count */
      hhcd->hc[chnum].ErrCnt = 0U;
    }
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_FRMOR))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_FRMOR);
    (void)USB_HC_Halt(hhcd->Instance, chnum);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_XFRC))
  {
    hhcd->hc[chnum].ErrCnt = 0U;

    /* transaction completed with NYET state, update do ping state */
    if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_NYET))
    {
      hhcd->hc[chnum].do_ping = 1U;
      __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NYET);
    }

    if (hhcd->hc[chnum].do_csplit != 0U)
    {
      hhcd->hc[chnum].do_csplit = 0U;
      __HAL_HCD_CLEAR_HC_CSPLT(chnum);
    }

    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_XFRC);
    hhcd->hc[chnum].state = HC_XFRC;
    (void)USB_HC_Halt(hhcd->Instance, chnum);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_NYET))
  {
    hhcd->hc[chnum].state = HC_NYET;

    if (hhcd->hc[chnum].do_ssplit == 0U)
    {
      hhcd->hc[chnum].do_ping = 1U;
    }

    hhcd->hc[chnum].ErrCnt = 0U;
    (void)USB_HC_Halt(hhcd->Instance, chnum);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NYET);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_STALL))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_STALL);
    hhcd->hc[chnum].state = HC_STALL;
    (void)USB_HC_Halt(hhcd->Instance, chnum);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_NAK))
  {
    hhcd->hc[chnum].ErrCnt = 0U;
    hhcd->hc[chnum].state = HC_NAK;

    if (hhcd->hc[chnum].do_ping == 0U)
    {
      if (hhcd->hc[chnum].speed == HCD_DEVICE_SPEED_HIGH)
      {
        hhcd->hc[chnum].do_ping = 1U;
      }
    }

    (void)USB_HC_Halt(hhcd->Instance, chnum);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_TXERR))
  {
    if (hhcd->Init.dma_enable == 0U)
    {
      hhcd->hc[chnum].state = HC_XACTERR;
      (void)USB_HC_Halt(hhcd->Instance, chnum);
    }
    else
    {
      hhcd->hc[chnum].ErrCnt++;
      if (hhcd->hc[chnum].ErrCnt > 2U)
      {
        hhcd->hc[chnum].ErrCnt = 0U;
        hhcd->hc[chnum].urb_state = URB_ERROR;

#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
        hhcd->HC_NotifyURBChangeCallback(hhcd, chnum, hhcd->hc[chnum].urb_state);
#else
        HAL_HCD_HC_NotifyURBChange_Callback(hhcd, chnum, hhcd->hc[chnum].urb_state);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */
      }
      else
      {
        hhcd->hc[chnum].urb_state = URB_NOTREADY;

        /* Re-activate the channel  */
        tmpreg = USBx_HC(chnum)->HCCHAR;
        tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
        tmpreg |= USB_OTG_HCCHAR_CHENA;
        USBx_HC(chnum)->HCCHAR = tmpreg;
      }
    }
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_TXERR);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_DTERR))
  {
    hhcd->hc[chnum].state = HC_DATATGLERR;
    (void)USB_HC_Halt(hhcd->Instance, chnum);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_DTERR);
  }
  else if (__HAL_HCD_GET_CH_FLAG(hhcd, chnum, USB_OTG_HCINT_CHH))
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_CHH);

    if (hhcd->hc[chnum].state == HC_XFRC)
    {
      hhcd->hc[chnum].state = HC_HALTED;
      hhcd->hc[chnum].urb_state = URB_DONE;

      if ((hhcd->hc[chnum].ep_type == EP_TYPE_BULK) ||
          (hhcd->hc[chnum].ep_type == EP_TYPE_INTR))
      {
        if (hhcd->Init.dma_enable == 0U)
        {
          hhcd->hc[chnum].toggle_out ^= 1U;
        }

        if ((hhcd->Init.dma_enable == 1U) && (hhcd->hc[chnum].xfer_len > 0U))
        {
          num_packets = (hhcd->hc[chnum].xfer_len + hhcd->hc[chnum].max_packet - 1U) / hhcd->hc[chnum].max_packet;

          if ((num_packets & 1U) != 0U)
          {
            hhcd->hc[chnum].toggle_out ^= 1U;
          }
        }
      }
    }
    else if (hhcd->hc[chnum].state == HC_ACK)
    {
      hhcd->hc[chnum].state = HC_HALTED;

      if (hhcd->hc[chnum].do_csplit == 1U)
      {
        hhcd->hc[chnum].urb_state = URB_NOTREADY;
      }
    }
    else if (hhcd->hc[chnum].state == HC_NAK)
    {
      hhcd->hc[chnum].state = HC_HALTED;
      hhcd->hc[chnum].urb_state = URB_NOTREADY;

      if (hhcd->hc[chnum].do_csplit == 1U)
      {
        hhcd->hc[chnum].do_csplit = 0U;
        __HAL_HCD_CLEAR_HC_CSPLT(chnum);
      }
    }
    else if (hhcd->hc[chnum].state == HC_NYET)
    {
      hhcd->hc[chnum].state = HC_HALTED;
      hhcd->hc[chnum].urb_state  = URB_NOTREADY;
    }
    else if (hhcd->hc[chnum].state == HC_STALL)
    {
      hhcd->hc[chnum].state = HC_HALTED;
      hhcd->hc[chnum].urb_state  = URB_STALL;
    }
    else if ((hhcd->hc[chnum].state == HC_XACTERR) ||
             (hhcd->hc[chnum].state == HC_DATATGLERR))
    {
      hhcd->hc[chnum].state = HC_HALTED;
      hhcd->hc[chnum].ErrCnt++;
      if (hhcd->hc[chnum].ErrCnt > 2U)
      {
        hhcd->hc[chnum].ErrCnt = 0U;
        hhcd->hc[chnum].urb_state = URB_ERROR;
      }
      else
      {
        hhcd->hc[chnum].urb_state = URB_NOTREADY;

        /* re-activate the channel  */
        tmpreg = USBx_HC(chnum)->HCCHAR;
        tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
        tmpreg |= USB_OTG_HCCHAR_CHENA;
        USBx_HC(chnum)->HCCHAR = tmpreg;
      }
    }
    else
    {
      return;
    }

#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
    hhcd->HC_NotifyURBChangeCallback(hhcd, chnum, hhcd->hc[chnum].urb_state);
#else
    HAL_HCD_HC_NotifyURBChange_Callback(hhcd, chnum, hhcd->hc[chnum].urb_state);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */
  }
  else
  {
    return;
  }
}

/**
  * @brief  Handle Rx Queue Level interrupt requests.
  * @param  hhcd HCD handle
  * @retval none
  */
static void HCD_RXQLVL_IRQHandler(HCD_HandleTypeDef *hhcd)
{
  const USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;
  uint32_t USBx_BASE = (uint32_t)USBx;
  uint32_t pktsts;
  uint32_t pktcnt;
  uint32_t GrxstspReg;
  uint32_t xferSizePktCnt;
  uint32_t tmpreg;
  uint32_t chnum;

  GrxstspReg = hhcd->Instance->GRXSTSP;
  chnum = GrxstspReg & USB_OTG_GRXSTSP_EPNUM;
  pktsts = (GrxstspReg & USB_OTG_GRXSTSP_PKTSTS) >> 17;
  pktcnt = (GrxstspReg & USB_OTG_GRXSTSP_BCNT) >> 4;

  switch (pktsts)
  {
    case GRXSTS_PKTSTS_IN:
      /* Read the data into the host buffer. */
      if ((pktcnt > 0U) && (hhcd->hc[chnum].xfer_buff != (void *)0))
      {
        if ((hhcd->hc[chnum].xfer_count + pktcnt) <= hhcd->hc[chnum].xfer_len)
        {
          (void)USB_ReadPacket(hhcd->Instance,
                               hhcd->hc[chnum].xfer_buff, (uint16_t)pktcnt);

          /* manage multiple Xfer */
          hhcd->hc[chnum].xfer_buff += pktcnt;
          hhcd->hc[chnum].xfer_count += pktcnt;

          /* get transfer size packet count */
          xferSizePktCnt = (USBx_HC(chnum)->HCTSIZ & USB_OTG_HCTSIZ_PKTCNT) >> 19;

          if ((hhcd->hc[chnum].max_packet == pktcnt) && (xferSizePktCnt > 0U))
          {
            /* re-activate the channel when more packets are expected */
            tmpreg = USBx_HC(chnum)->HCCHAR;
            tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
            tmpreg |= USB_OTG_HCCHAR_CHENA;
            USBx_HC(chnum)->HCCHAR = tmpreg;
            hhcd->hc[chnum].toggle_in ^= 1U;
          }
        }
        else
        {
          hhcd->hc[chnum].urb_state = URB_ERROR;
        }
      }
      break;

    case GRXSTS_PKTSTS_DATA_TOGGLE_ERR:
      break;

    case GRXSTS_PKTSTS_IN_XFER_COMP:
    case GRXSTS_PKTSTS_CH_HALTED:
    default:
      break;
  }
}

/**
  * @brief  Handle Host Port interrupt requests.
  * @param  hhcd HCD handle
  * @retval None
  */
static void HCD_Port_IRQHandler(HCD_HandleTypeDef *hhcd)
{
  const USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;
  uint32_t USBx_BASE = (uint32_t)USBx;
  __IO uint32_t hprt0;
  __IO uint32_t hprt0_dup;

  /* Handle Host Port Interrupts */
  hprt0 = USBx_HPRT0;
  hprt0_dup = USBx_HPRT0;

  hprt0_dup &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET | \
                 USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG);

  /* Check whether Port Connect detected */
  if ((hprt0 & USB_OTG_HPRT_PCDET) == USB_OTG_HPRT_PCDET)
  {
    if ((hprt0 & USB_OTG_HPRT_PCSTS) == USB_OTG_HPRT_PCSTS)
    {
#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
      hhcd->ConnectCallback(hhcd);
#else
      HAL_HCD_Connect_Callback(hhcd);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */
    }
    hprt0_dup |= USB_OTG_HPRT_PCDET;
  }

  /* Check whether Port Enable Changed */
  if ((hprt0 & USB_OTG_HPRT_PENCHNG) == USB_OTG_HPRT_PENCHNG)
  {
    hprt0_dup |= USB_OTG_HPRT_PENCHNG;

    if ((hprt0 & USB_OTG_HPRT_PENA) == USB_OTG_HPRT_PENA)
    {
#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
      hhcd->PortEnabledCallback(hhcd);
#else
      HAL_HCD_PortEnabled_Callback(hhcd);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */

    }
    else
    {
#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
      hhcd->PortDisabledCallback(hhcd);
#else
      HAL_HCD_PortDisabled_Callback(hhcd);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */
    }
  }

  /* Check for an overcurrent */
  if ((hprt0 & USB_OTG_HPRT_POCCHNG) == USB_OTG_HPRT_POCCHNG)
  {
    hprt0_dup |= USB_OTG_HPRT_POCCHNG;
  }

  /* Clear Port Interrupts */
  USBx_HPRT0 = hprt0_dup;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* defined (USB1_OTG_HS) || defined (USB2_OTG_HS) */
#endif /* HAL_HCD_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */
