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

#include "usbd_hid_interface.h"

#include "py/obj.h"
#include "irq.h"
#include "usb.h"

uint8_t *usbd_hid_init(usbd_hid_itf_t *hid, usbd_cdc_msc_hid_state_t *usbd) {
    hid->usbd = usbd;
    hid->current_read_buffer = 0;
    hid->last_read_len = 0;
    hid->current_write_buffer = 0;

    // Return the buffer to place the first USB OUT packet
    return hid->buffer[hid->current_write_buffer];
}

// Data received over USB OUT endpoint is processed here.
// len: number of bytes received into the buffer we passed to USBD_HID_ReceivePacket
// Returns USBD_OK if all operations are OK else USBD_FAIL
int8_t usbd_hid_receive(usbd_hid_itf_t *hid, size_t len) {
    hid->current_write_buffer = !hid->current_write_buffer;
    hid->last_read_len = len;
    // initiate next USB packet transfer, to append to existing data in buffer
    USBD_HID_ReceivePacket(hid->usbd, hid->buffer[hid->current_write_buffer]);
    // Set NAK to indicate we need to process read buffer
    USBD_HID_SetNAK(hid->usbd);
    return USBD_OK;
}

// Returns number of ready rx buffers.
int usbd_hid_rx_num(usbd_hid_itf_t *hid) {
    return hid->current_read_buffer != hid->current_write_buffer;
}

// timout in milliseconds.
// Returns number of bytes read from the device.
int usbd_hid_rx(usbd_hid_itf_t *hid, size_t len, uint8_t *buf, uint32_t timeout) {
    // Wait until we have buffer to read
    uint32_t start = HAL_GetTick();
    while (hid->current_read_buffer == hid->current_write_buffer) {
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
    if (len < hid->last_read_len) {
        return 0;
    }

    // Copy bytes from device to user buffer
    memcpy(buf, hid->buffer[hid->current_read_buffer], hid->last_read_len);
    hid->current_read_buffer = !hid->current_read_buffer;

    // Clear NAK to indicate we are ready to read more data
    USBD_HID_ClearNAK(hid->usbd);

    // Success, return number of bytes read
    return hid->last_read_len;
}
