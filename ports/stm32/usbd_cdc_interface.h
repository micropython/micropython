/*
 * This file is part of the MicroPython project, http://micropython.org/
 */
#ifndef MICROPY_INCLUDED_STM32_USBD_CDC_INTERFACE_H
#define MICROPY_INCLUDED_STM32_USBD_CDC_INTERFACE_H

/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Inc/usbd_cdc_interface.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    26-February-2014
  * @brief   Header for usbd_cdc_interface.c file.
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

#include "py/mpconfig.h"

// Values for connect_state
#define USBD_CDC_CONNECT_STATE_DISCONNECTED (0)
#define USBD_CDC_CONNECT_STATE_CONNECTING (1)
#define USBD_CDC_CONNECT_STATE_CONNECTED (2)

// Flow control settings
#define USBD_CDC_FLOWCONTROL_NONE (0)
#define USBD_CDC_FLOWCONTROL_RTS (1)
#define USBD_CDC_FLOWCONTROL_CTS (2)

typedef struct _usbd_cdc_itf_t {
    usbd_cdc_state_t base; // state for the base CDC layer

    uint8_t rx_packet_buf[CDC_DATA_MAX_PACKET_SIZE]; // received data from USB OUT endpoint is stored in this buffer
    uint8_t rx_user_buf[MICROPY_HW_USB_CDC_RX_DATA_SIZE]; // received data is buffered here until the user reads it
    volatile uint16_t rx_buf_put; // circular buffer index
    uint16_t rx_buf_get; // circular buffer index
    uint8_t rx_buf_full; // rx from host will be blocked while this is true

    uint8_t tx_buf[MICROPY_HW_USB_CDC_TX_DATA_SIZE]; // data for USB IN endpoind is stored in this buffer
    uint16_t tx_buf_ptr_in; // increment this pointer modulo MICROPY_HW_USB_CDC_TX_DATA_SIZE when new data is available
    volatile uint16_t tx_buf_ptr_out; // increment this pointer modulo MICROPY_HW_USB_CDC_TX_DATA_SIZE when data is drained
    uint16_t tx_buf_ptr_out_next; // next position of above once transmission finished
    uint8_t tx_need_empty_packet; // used to flush the USB IN endpoint if the last packet was exactly the endpoint packet size

    uint8_t cdc_idx; // between 0 and MICROPY_HW_USB_CDC_NUM-1
    volatile uint8_t connect_state; // indicates if we are connected
    uint8_t attached_to_repl; // indicates if interface is connected to REPL
    uint8_t flow; // USBD_CDC_FLOWCONTROL_* setting flags
} usbd_cdc_itf_t;

// This is implemented in usb.c
usbd_cdc_itf_t *usb_vcp_get(int idx);

static inline int usbd_cdc_is_connected(usbd_cdc_itf_t *cdc) {
    return cdc->connect_state == USBD_CDC_CONNECT_STATE_CONNECTED;
}

int usbd_cdc_tx_half_empty(usbd_cdc_itf_t *cdc);
int usbd_cdc_tx_flow(usbd_cdc_itf_t *cdc, const uint8_t *buf, uint32_t len);
int usbd_cdc_tx(usbd_cdc_itf_t *cdc, const uint8_t *buf, uint32_t len, uint32_t timeout);
void usbd_cdc_tx_always(usbd_cdc_itf_t *cdc, const uint8_t *buf, uint32_t len);

int usbd_cdc_rx_num(usbd_cdc_itf_t *cdc);
int usbd_cdc_rx(usbd_cdc_itf_t *cdc, uint8_t *buf, uint32_t len, uint32_t timeout);
void usbd_cdc_rx_event_callback(usbd_cdc_itf_t *cdc);

#endif // MICROPY_INCLUDED_STM32_USBD_CDC_INTERFACE_H
