/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Taken from ST Cube library and heavily modified.  See below for original
 * copyright header.
 */

/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Src/usbd_cdc_interface.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    26-February-2014
  * @brief   Source file for USBD CDC interface
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

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

#include "usbd_cdc_msc_hid.h"
#include "usbd_hid_interface.h"

#include "py/obj.h"
#include "irq.h"
#include "usb.h"

/* Private variables ---------------------------------------------------------*/

static __IO uint8_t dev_is_connected = 0; // indicates if we are connected

static uint8_t buffer[2][HID_DATA_FS_MAX_PACKET_SIZE]; // pair of buffers to read individual packets into
static int8_t current_read_buffer = 0; // which buffer to read from
static uint32_t last_read_len; // length of last read
static int8_t current_write_buffer = 0; // which buffer to write to


/* Private function prototypes -----------------------------------------------*/
static int8_t HID_Itf_Receive  (uint8_t* pbuf, uint32_t Len);

const USBD_HID_ItfTypeDef USBD_HID_fops = {
    HID_Itf_Receive
};

/**
  * @brief  HID_Itf_Receive
  *         Data received over USB OUT endpoint is processed here.
  * @param  Buf: Buffer of data received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  * @note   The buffer we are passed here is just UserRxBuffer, so we are
  *         free to modify it.
  */
static int8_t HID_Itf_Receive(uint8_t* Buf, uint32_t Len) {
    current_write_buffer = !current_write_buffer;
    last_read_len = Len;
    // initiate next USB packet transfer, to append to existing data in buffer
    USBD_HID_SetRxBuffer(&hUSBDDevice, buffer[current_write_buffer]);
    USBD_HID_ReceivePacket(&hUSBDDevice);

    return USBD_OK;
}

// timout in milliseconds.
// Returns number of bytes read from the device.
int USBD_HID_Rx(uint8_t *buf, uint32_t len, uint32_t timeout) {
    // Wait until we have buffer to read
    uint32_t start = HAL_GetTick();
    while (current_read_buffer == current_write_buffer) {
        // Wraparound of tick is taken care of by 2's complement arithmetic.
        if (HAL_GetTick() - start >= timeout) {
            // timeout
            return 0;
        }
        if (query_irq() == IRQ_STATE_DISABLED) {
            // IRQs disabled so buffer will never be filled; return immediately
            return 0;
        }
        __WFI(); // enter sleep mode, waiting for interrupt
    }

    // There is not enough space in buffer
    if (len < last_read_len) {
        return 0;
    }

    // Copy bytes from device to user buffer
    memcpy(buf, buffer[current_read_buffer], last_read_len);
    current_read_buffer = !current_read_buffer;

    // Success, return number of bytes read
    return last_read_len;
}
