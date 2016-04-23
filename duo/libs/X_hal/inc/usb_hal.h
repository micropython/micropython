/**
 ******************************************************************************
 * @file    usb_hal.h
 * @author  Satish Nair
 * @version V1.0.0
 * @date    10-Sept-2014
 * @brief   USB Virtual COM Port and HID device HAL
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_HAL_H
#define __USB_HAL_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#include "usb_config_hal.h"


    /* USB Config : IMR_MSK */
/* mask defining which events has to be handled */
/* by the device application software */
#define IMR_MSK (CNTR_CTRM  | \
                 CNTR_WKUPM | \
                 CNTR_SUSPM | \
                 CNTR_ERRM  | \
                 CNTR_SOFM  | \
                 CNTR_ESOFM | \
                 CNTR_RESETM  \
)

#define USART_RX_DATA_SIZE      256

/* Exported functions ------------------------------------------------------- */
#if defined (USB_CDC_ENABLE) || defined (USB_HID_ENABLE)
void SPARK_USB_Setup(void);
void Get_SerialNum(void);
#endif

#ifdef USB_CDC_ENABLE
/**
 * Initialize or deinitialize USB serial
 * @param baudRate  The data rate of the connection. If 0, the connection is
 * uninitialized.
 */
void USB_USART_Init(uint32_t baudRate);

unsigned int USB_USART_Baud_Rate(void);

void USB_USART_LineCoding_BitRate_Handler(void (*handler)(uint32_t bitRate));

/**
 * Retrieves the number of bytes of data available.
 * @return
 */
uint8_t USB_USART_Available_Data(void);

/**
 * Reads data from the input buffer.
 * @param peek  If the data should be peeked reather than fetched.
 * The default, `false` means fetch, where data is removed from the buffer.
 * When `true`, the data byte is left in the buffer.
 * @return
 */
int32_t USB_USART_Receive_Data(uint8_t peek);

/**
 * Sends data to the USB serial.
 * @param Data      The data to write.
 */
void USB_USART_Send_Data(uint8_t Data);
#endif

#ifdef USB_HID_ENABLE
void USB_HID_Send_Report(void *pHIDReport, size_t reportSize);
#endif

#ifdef __cplusplus
}
#endif


#endif  /* __USB_HAL_H */
