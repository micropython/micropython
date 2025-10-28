/*
 * This file is part of the MicroPython project, http://micropython.org/
 */

/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Inc/usbd_conf.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    26-February-2014
  * @brief   General low level driver configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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

#ifndef MICROPY_INCLUDED_STM32_USBD_CONF_H
#define MICROPY_INCLUDED_STM32_USBD_CONF_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "py/mpconfig.h"

#define USBD_MAX_NUM_INTERFACES               8
#define USBD_MAX_NUM_CONFIGURATION            1
#define USBD_MAX_STR_DESC_SIZ                 0x100
#if MICROPY_HW_USB_SELF_POWERED
#define USBD_SELF_POWERED                     1
#else
#define USBD_SELF_POWERED                     0
#endif
#define USBD_DEBUG_LEVEL                      0

// Whether the core USBD driver passes through vendor device requests to the class implementation.
#ifndef USBD_ENABLE_VENDOR_DEVICE_REQUESTS
#define USBD_ENABLE_VENDOR_DEVICE_REQUESTS    (0)
#endif

// For MCUs with a device-only USB peripheral
#define USBD_PMA_RESERVE                      (64)
#define USBD_PMA_NUM_FIFO                     (16) // Maximum 8 endpoints, 2 FIFOs each

// For MCUs with multiple OTG USB peripherals
#define USBD_FS_NUM_TX_FIFO                   (6)
#define USBD_FS_NUM_FIFO                      (1 + USBD_FS_NUM_TX_FIFO)
#define USBD_HS_NUM_TX_FIFO                   (9)
#define USBD_HS_NUM_FIFO                      (1 + USBD_HS_NUM_TX_FIFO)

#if MICROPY_HW_TINYUSB_STACK
void pyb_usbd_init(void);
#endif

#endif // MICROPY_INCLUDED_STM32_USBD_CONF_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
