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

#include <stdbool.h>
#include <stdint.h>

#include "usbd_cdc_msc_hid.h"
#include "usbd_cdc_interface.h"
#include "pendsv.h"

#include "py/mpstate.h"
#include "py/obj.h"
#include "lib/utils/interrupt_char.h"
#include "irq.h"
#include "timer.h"
#include "usb.h"

// CDC control commands
#define CDC_SEND_ENCAPSULATED_COMMAND               0x00
#define CDC_GET_ENCAPSULATED_RESPONSE               0x01
#define CDC_SET_COMM_FEATURE                        0x02
#define CDC_GET_COMM_FEATURE                        0x03
#define CDC_CLEAR_COMM_FEATURE                      0x04
#define CDC_SET_LINE_CODING                         0x20
#define CDC_GET_LINE_CODING                         0x21
#define CDC_SET_CONTROL_LINE_STATE                  0x22
#define CDC_SEND_BREAK                              0x23

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define APP_RX_DATA_SIZE  1024 // this must be 2 or greater, and a power of 2
#define APP_TX_DATA_SIZE  1024 // I think this can be any value (was 2048)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static __IO uint8_t dev_is_connected = 0; // indicates if we are connected

static uint8_t cdc_rx_packet_buf[CDC_DATA_FS_MAX_PACKET_SIZE]; // received data from USB OUT endpoint is stored in this buffer
static uint8_t cdc_rx_user_buf[APP_RX_DATA_SIZE]; // received data is buffered here until the user reads it
static volatile uint16_t cdc_rx_buf_put = 0; // circular buffer index
static uint16_t cdc_rx_buf_get = 0; // circular buffer index

static uint8_t UserTxBuffer[APP_TX_DATA_SIZE]; // data for USB IN endpoind is stored in this buffer
static uint16_t UserTxBufPtrIn = 0; // increment this pointer modulo APP_TX_DATA_SIZE when new data is available
static __IO uint16_t UserTxBufPtrOut = 0; // increment this pointer modulo APP_TX_DATA_SIZE when data is drained
static uint16_t UserTxBufPtrOutShadow = 0; // shadow of above
static uint8_t UserTxBufPtrWaitCount = 0; // used to implement a timeout waiting for low-level USB driver
static uint8_t UserTxNeedEmptyPacket = 0; // used to flush the USB IN endpoint if the last packet was exactly the endpoint packet size

/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_Itf_Init     (USBD_HandleTypeDef *pdev);
static int8_t CDC_Itf_DeInit   (void);
static int8_t CDC_Itf_Control  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Itf_Receive  (USBD_HandleTypeDef *pdev, uint8_t* pbuf, uint32_t *Len);

const USBD_CDC_ItfTypeDef USBD_CDC_fops = {
    CDC_Itf_Init,
    CDC_Itf_DeInit,
    CDC_Itf_Control,
    CDC_Itf_Receive
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  CDC_Itf_Init
  *         Initializes the CDC media low layer
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Init(USBD_HandleTypeDef *pdev) {
    USBD_CDC_SetTxBuffer(pdev, UserTxBuffer, 0);
    USBD_CDC_SetRxBuffer(pdev, cdc_rx_packet_buf);

    cdc_rx_buf_put = 0;
    cdc_rx_buf_get = 0;

    return USBD_OK;
}

/**
  * @brief  CDC_Itf_DeInit
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_DeInit(void) {
    return USBD_OK;
}

/**
  * @brief  CDC_Itf_Control
  *         Manage the CDC class requests
  * @param  Cmd: Command code
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length) {
    switch (cmd) {
        case CDC_SEND_ENCAPSULATED_COMMAND:
            /* Add your code here */
            break;

        case CDC_GET_ENCAPSULATED_RESPONSE:
            /* Add your code here */
            break;

        case CDC_SET_COMM_FEATURE:
            /* Add your code here */
            break;

        case CDC_GET_COMM_FEATURE:
            /* Add your code here */
            break;

        case CDC_CLEAR_COMM_FEATURE:
            /* Add your code here */
            break;

        case CDC_SET_LINE_CODING:
            #if 0
            LineCoding.bitrate    = (uint32_t)(pbuf[0] | (pbuf[1] << 8) |\
                                    (pbuf[2] << 16) | (pbuf[3] << 24));
            LineCoding.format     = pbuf[4];
            LineCoding.paritytype = pbuf[5];
            LineCoding.datatype   = pbuf[6];
            /* Set the new configuration */
            #endif
            break;

        case CDC_GET_LINE_CODING:
            /* Add your code here */
            pbuf[0] = (uint8_t)(115200);
            pbuf[1] = (uint8_t)(115200 >> 8);
            pbuf[2] = (uint8_t)(115200 >> 16);
            pbuf[3] = (uint8_t)(115200 >> 24);
            pbuf[4] = 0; // stop bits (1)
            pbuf[5] = 0; // parity (none)
            pbuf[6] = 8; // number of bits (8)
            break;

        case CDC_SET_CONTROL_LINE_STATE:
            dev_is_connected = length & 1; // wValue is passed in Len (bit of a hack)
            break;

        case CDC_SEND_BREAK:
            /* Add your code here */
            break;

        default:
            break;
    }

    return USBD_OK;
}

// This function is called to process outgoing data.  We hook directly into the
// SOF (start of frame) callback so that it is called exactly at the time it is
// needed (reducing latency), and often enough (increasing bandwidth).
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd) {
    if (!dev_is_connected) {
        // CDC device is not connected to a host, so we are unable to send any data
        return;
    }

    if (UserTxBufPtrOut == UserTxBufPtrIn && !UserTxNeedEmptyPacket) {
        // No outstanding data to send
        return;
    }

    if (UserTxBufPtrOut != UserTxBufPtrOutShadow) {
        // We have sent data and are waiting for the low-level USB driver to
        // finish sending it over the USB in-endpoint.
        // SOF occurs every 1ms, so we have a 500 * 1ms = 500ms timeout
        // We have a relatively large timeout because the USB host may be busy
        // doing other things and we must give it a chance to read our data.
        if (UserTxBufPtrWaitCount < 500) {
            USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
            if (USBx_INEP(CDC_IN_EP & 0x7f)->DIEPTSIZ & USB_OTG_DIEPTSIZ_XFRSIZ) {
                // USB in-endpoint is still reading the data
                UserTxBufPtrWaitCount++;
                return;
            }
        }
        UserTxBufPtrOut = UserTxBufPtrOutShadow;
    }

    if (UserTxBufPtrOutShadow != UserTxBufPtrIn || UserTxNeedEmptyPacket) {
        uint32_t buffptr;
        uint32_t buffsize;

        if (UserTxBufPtrOutShadow > UserTxBufPtrIn) { // rollback
            buffsize = APP_TX_DATA_SIZE - UserTxBufPtrOutShadow;
        } else {
            buffsize = UserTxBufPtrIn - UserTxBufPtrOutShadow;
        }

        buffptr = UserTxBufPtrOutShadow;

        USBD_CDC_SetTxBuffer(hpcd->pData, (uint8_t*)&UserTxBuffer[buffptr], buffsize);

        if (USBD_CDC_TransmitPacket(hpcd->pData) == USBD_OK) {
            UserTxBufPtrOutShadow += buffsize;
            if (UserTxBufPtrOutShadow == APP_TX_DATA_SIZE) {
                UserTxBufPtrOutShadow = 0;
            }
            UserTxBufPtrWaitCount = 0;

            // According to the USB specification, a packet size of 64 bytes (CDC_DATA_FS_MAX_PACKET_SIZE)
            // gets held at the USB host until the next packet is sent.  This is because a
            // packet of maximum size is considered to be part of a longer chunk of data, and
            // the host waits for all data to arrive (ie, waits for a packet < max packet size).
            // To flush a packet of exactly max packet size, we need to send a zero-size packet.
            // See eg http://www.cypress.com/?id=4&rID=92719
            UserTxNeedEmptyPacket = (buffsize > 0 && buffsize % CDC_DATA_FS_MAX_PACKET_SIZE == 0 && UserTxBufPtrOutShadow == UserTxBufPtrIn);
        }
    }
}

/**
  * @brief  CDC_Itf_DataRx
  *         Data received over USB OUT endpoint is processed here.
  * @param  Buf: Buffer of data received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  * @note   The buffer we are passed here is just cdc_rx_packet_buf, so we are
  *         free to modify it.
  */
static int8_t CDC_Itf_Receive(USBD_HandleTypeDef *pdev, uint8_t* Buf, uint32_t *Len) {
    // copy the incoming data into the circular buffer
    for (uint8_t *src = Buf, *top = Buf + *Len; src < top; ++src) {
        if (mp_interrupt_char != -1 && *src == mp_interrupt_char) {
            pendsv_kbd_intr();
        } else {
            uint16_t next_put = (cdc_rx_buf_put + 1) & (APP_RX_DATA_SIZE - 1);
            if (next_put == cdc_rx_buf_get) {
                // overflow, we just discard the rest of the chars
                break;
            }
            cdc_rx_user_buf[cdc_rx_buf_put] = *src;
            cdc_rx_buf_put = next_put;
        }
    }

    // initiate next USB packet transfer
    USBD_CDC_SetRxBuffer(pdev, cdc_rx_packet_buf);
    USBD_CDC_ReceivePacket(pdev);

    return USBD_OK;
}

int USBD_CDC_IsConnected(void) {
    return dev_is_connected;
}

int USBD_CDC_TxHalfEmpty(void) {
    int32_t tx_waiting = (int32_t)UserTxBufPtrIn - (int32_t)UserTxBufPtrOut;
    if (tx_waiting < 0) {
        tx_waiting += APP_TX_DATA_SIZE;
    }
    return tx_waiting <= APP_TX_DATA_SIZE / 2;
}

// timout in milliseconds.
// Returns number of bytes written to the device.
int USBD_CDC_Tx(const uint8_t *buf, uint32_t len, uint32_t timeout) {
    for (uint32_t i = 0; i < len; i++) {
        // Wait until the device is connected and the buffer has space, with a given timeout
        uint32_t start = HAL_GetTick();
        while (!dev_is_connected || ((UserTxBufPtrIn + 1) & (APP_TX_DATA_SIZE - 1)) == UserTxBufPtrOut) {
            // Wraparound of tick is taken care of by 2's complement arithmetic.
            if (HAL_GetTick() - start >= timeout) {
                // timeout
                return i;
            }
            if (query_irq() == IRQ_STATE_DISABLED) {
                // IRQs disabled so buffer will never be drained; return immediately
                return i;
            }
            __WFI(); // enter sleep mode, waiting for interrupt
        }

        // Write data to device buffer
        UserTxBuffer[UserTxBufPtrIn] = buf[i];
        UserTxBufPtrIn = (UserTxBufPtrIn + 1) & (APP_TX_DATA_SIZE - 1);
    }

    // Success, return number of bytes read
    return len;
}

// Always write all of the data to the device tx buffer, even if the
// device is not connected, or if the buffer is full.  Has a small timeout
// to wait for the buffer to be drained, in the case the device is connected.
void USBD_CDC_TxAlways(const uint8_t *buf, uint32_t len) {
    for (int i = 0; i < len; i++) {
        // If the CDC device is not connected to the host then we don't have anyone to receive our data.
        // The device may become connected in the future, so we should at least try to fill the buffer
        // and hope that it doesn't overflow by the time the device connects.
        // If the device is not connected then we should go ahead and fill the buffer straight away,
        // ignoring overflow.  Otherwise, we should make sure that we have enough room in the buffer.
        if (dev_is_connected) {
            // If the buffer is full, wait until it gets drained, with a timeout of 500ms
            // (wraparound of tick is taken care of by 2's complement arithmetic).
            uint32_t start = HAL_GetTick();
            while (((UserTxBufPtrIn + 1) & (APP_TX_DATA_SIZE - 1)) == UserTxBufPtrOut && HAL_GetTick() - start <= 500) {
                if (query_irq() == IRQ_STATE_DISABLED) {
                    // IRQs disabled so buffer will never be drained; exit loop
                    break;
                }
                __WFI(); // enter sleep mode, waiting for interrupt
            }

            // Some unused code that makes sure the low-level USB buffer is drained.
            // Waiting for low-level is handled in HAL_PCD_SOFCallback.
            /*
            start = HAL_GetTick();
            PCD_HandleTypeDef *hpcd = hUSBDDevice.pData;
            if (hpcd->IN_ep[0x83 & 0x7f].is_in) {
                //volatile uint32_t *xfer_count = &hpcd->IN_ep[0x83 & 0x7f].xfer_count;
                //volatile uint32_t *xfer_len = &hpcd->IN_ep[0x83 & 0x7f].xfer_len;
                USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
                while (
                    // *xfer_count < *xfer_len // using this works
                    // (USBx_INEP(3)->DIEPTSIZ & USB_OTG_DIEPTSIZ_XFRSIZ) // using this works
                    && HAL_GetTick() - start <= 2000) {
                    __WFI(); // enter sleep mode, waiting for interrupt
                }
            }
            */
        }

        UserTxBuffer[UserTxBufPtrIn] = buf[i];
        UserTxBufPtrIn = (UserTxBufPtrIn + 1) & (APP_TX_DATA_SIZE - 1);
    }
}

// Returns number of bytes in the rx buffer.
int USBD_CDC_RxNum(void) {
    int32_t rx_waiting = (int32_t)cdc_rx_buf_put - (int32_t)cdc_rx_buf_get;
    if (rx_waiting < 0) {
        rx_waiting += APP_RX_DATA_SIZE;
    }
    return rx_waiting;
}

// timout in milliseconds.
// Returns number of bytes read from the device.
int USBD_CDC_Rx(uint8_t *buf, uint32_t len, uint32_t timeout) {
    // loop to read bytes
    for (uint32_t i = 0; i < len; i++) {
        // Wait until we have at least 1 byte to read
        uint32_t start = HAL_GetTick();
        while (cdc_rx_buf_put == cdc_rx_buf_get) {
            // Wraparound of tick is taken care of by 2's complement arithmetic.
            if (HAL_GetTick() - start >= timeout) {
                // timeout
                return i;
            }
            if (query_irq() == IRQ_STATE_DISABLED) {
                // IRQs disabled so buffer will never be filled; return immediately
                return i;
            }
            __WFI(); // enter sleep mode, waiting for interrupt
        }

        // Copy byte from device to user buffer
        buf[i] = cdc_rx_user_buf[cdc_rx_buf_get];
        cdc_rx_buf_get = (cdc_rx_buf_get + 1) & (APP_RX_DATA_SIZE - 1);
    }

    // Success, return number of bytes read
    return len;
}
