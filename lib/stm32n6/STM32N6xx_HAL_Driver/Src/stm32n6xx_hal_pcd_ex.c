/**
  ******************************************************************************
  * @file    stm32n6xx_hal_pcd_ex.c
  * @author  MCD Application Team
  * @brief   PCD Extended HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the USB Peripheral Controller:
  *           + Extended features functions
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
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup PCDEx PCDEx
  * @brief PCD Extended HAL module driver
  * @{
  */

#ifdef HAL_PCD_MODULE_ENABLED

#if defined (USB1_OTG_HS) || defined (USB2_OTG_HS)
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup PCDEx_Exported_Functions PCDEx Exported Functions
  * @{
  */

/** @defgroup PCDEx_Exported_Functions_Group1 Peripheral Control functions
  * @brief    PCDEx control functions
  *
@verbatim
 ===============================================================================
                 ##### Extended features functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Update FIFO configuration

@endverbatim
  * @{
  */
#if defined (USB1_OTG_HS) || defined (USB2_OTG_HS)
/**
  * @brief  Set Tx FIFO
  * @param  hpcd PCD handle
  * @param  fifo The number of Tx fifo
  * @param  size Fifo size
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_SetTxFiFo(PCD_HandleTypeDef *hpcd, uint8_t fifo, uint16_t size)
{
  uint8_t i;
  uint32_t Tx_Offset;

  /*  TXn min size = 16 words. (n  : Transmit FIFO index)
      When a TxFIFO is not used, the Configuration should be as follows:
          case 1 :  n > m    and Txn is not used    (n,m  : Transmit FIFO indexes)
         --> Txm can use the space allocated for Txn.
         case2  :  n < m    and Txn is not used    (n,m  : Transmit FIFO indexes)
         --> Txn should be configured with the minimum space of 16 words
     The FIFO is used optimally when used TxFIFOs are allocated in the top
         of the FIFO.Ex: use EP1 and EP2 as IN instead of EP1 and EP3 as IN ones.
     When DMA is used 3n * FIFO locations should be reserved for internal DMA registers */

  Tx_Offset = hpcd->Instance->GRXFSIZ;

  if (fifo == 0U)
  {
    hpcd->Instance->DIEPTXF0_HNPTXFSIZ = ((uint32_t)size << 16) | Tx_Offset;
  }
  else
  {
    Tx_Offset += (hpcd->Instance->DIEPTXF0_HNPTXFSIZ) >> 16;
    for (i = 0U; i < (fifo - 1U); i++)
    {
      Tx_Offset += (hpcd->Instance->DIEPTXF[i] >> 16);
    }

    /* Multiply Tx_Size by 2 to get higher performance */
    hpcd->Instance->DIEPTXF[fifo - 1U] = ((uint32_t)size << 16) | Tx_Offset;
  }

  return HAL_OK;
}

/**
  * @brief  Set Rx FIFO
  * @param  hpcd PCD handle
  * @param  size Size of Rx fifo
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_SetRxFiFo(PCD_HandleTypeDef *hpcd, uint16_t size)
{
  hpcd->Instance->GRXFSIZ = size;

  return HAL_OK;
}

/**
  * @brief  Activate LPM feature.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_ActivateLPM(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

  hpcd->lpm_active = 1U;
  hpcd->LPM_State = LPM_L0;
  USBx->GINTMSK |= USB_OTG_GINTMSK_LPMINTM;
  USBx->GLPMCFG |= (USB_OTG_GLPMCFG_LPMEN | USB_OTG_GLPMCFG_LPMACK | USB_OTG_GLPMCFG_ENBESL);

  return HAL_OK;
}

/**
  * @brief  Deactivate LPM feature.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_DeActivateLPM(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

  hpcd->lpm_active = 0U;
  USBx->GINTMSK &= ~USB_OTG_GINTMSK_LPMINTM;
  USBx->GLPMCFG &= ~(USB_OTG_GLPMCFG_LPMEN | USB_OTG_GLPMCFG_LPMACK | USB_OTG_GLPMCFG_ENBESL);

  return HAL_OK;
}


/**
  * @brief  Handle BatteryCharging Process.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
void HAL_PCDEx_BCD_VBUSDetect(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
  uint32_t tickstart = HAL_GetTick();

  /* Enable DCD : Data Contact Detect */
  USBx->GCCFG |= USB_OTG_GCCFG_DCDETEN;

  /* Wait for Min DCD Timeout */
  HAL_Delay(300U);


  /* Primary detection: checks if connected to Standard Downstream Port
  (without charging capability) */
  USBx->GCCFG &= ~USB_OTG_GCCFG_DCDETEN;
  HAL_Delay(50U);
  USBx->GCCFG |= USB_OTG_GCCFG_PDETEN;
  HAL_Delay(50U);

  if ((USBx->GCCFG & USB_OTG_GCCFG_CHGDET) == 0U)
  {
    /* Case of Standard Downstream Port */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
    hpcd->BCDCallback(hpcd, PCD_BCD_STD_DOWNSTREAM_PORT);
#else
    HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_STD_DOWNSTREAM_PORT);
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
  }
  else
  {
    /* start secondary detection to check connection to Charging Downstream
    Port or Dedicated Charging Port */
    USBx->GCCFG &= ~(USB_OTG_GCCFG_PDETEN);
    HAL_Delay(50U);
    USBx->GCCFG |= USB_OTG_GCCFG_SDETEN;
    HAL_Delay(50U);

    if ((USBx->GCCFG & USB_OTG_GCCFG_FSVPLUS) == USB_OTG_GCCFG_FSVPLUS)
    {
      /* case Dedicated Charging Port  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
      hpcd->BCDCallback(hpcd, PCD_BCD_DEDICATED_CHARGING_PORT);
#else
      HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_DEDICATED_CHARGING_PORT);
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
    }
    else
    {
      /* case Charging Downstream Port */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
      hpcd->BCDCallback(hpcd, PCD_BCD_CHARGING_DOWNSTREAM_PORT);
#else
      HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_CHARGING_DOWNSTREAM_PORT);
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
    }
  }

  /* Battery Charging capability discovery finished */
  (void)HAL_PCDEx_DeActivateBCD(hpcd);

  /* Check for the Timeout, else start USB Device */
  if ((HAL_GetTick() - tickstart) > 1000U)
  {
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
    hpcd->BCDCallback(hpcd, PCD_BCD_ERROR);
#else
    HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_ERROR);
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
  }
  else
  {
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
    hpcd->BCDCallback(hpcd, PCD_BCD_DISCOVERY_COMPLETED);
#else
    HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_DISCOVERY_COMPLETED);
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
  }
}

/**
  * @brief  Activate BatteryCharging feature.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_ActivateBCD(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

  USBx->GCCFG &= ~(USB_OTG_GCCFG_PDETEN);
  USBx->GCCFG &= ~(USB_OTG_GCCFG_SDETEN);

  hpcd->battery_charging_active = 1U;

  return HAL_OK;
}

/**
  * @brief  Deactivate BatteryCharging feature.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_DeActivateBCD(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

  USBx->GCCFG &= ~(USB_OTG_GCCFG_PDETEN);
  USBx->GCCFG &= ~(USB_OTG_GCCFG_SDETEN);

  hpcd->battery_charging_active = 0U;

  return HAL_OK;
}

#endif /* defined (USB1_OTG_HS) || defined (USB2_OTG_HS) */

/**
  * @brief  Send LPM message to user layer callback.
  * @param  hpcd PCD handle
  * @param  msg LPM message
  * @retval HAL status
  */
__weak void HAL_PCDEx_LPM_Callback(PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  UNUSED(msg);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PCDEx_LPM_Callback could be implemented in the user file
   */
}

/**
  * @brief  Send BatteryCharging message to user layer callback.
  * @param  hpcd PCD handle
  * @param  msg LPM message
  * @retval HAL status
  */
__weak void HAL_PCDEx_BCD_Callback(PCD_HandleTypeDef *hpcd, PCD_BCD_MsgTypeDef msg)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  UNUSED(msg);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PCDEx_BCD_Callback could be implemented in the user file
   */
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* defined (USB1_OTG_HS) || defined (USB2_OTG_HS) */
#endif /* HAL_PCD_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */
