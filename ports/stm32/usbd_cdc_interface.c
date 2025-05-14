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

#include "boardctrl.h"
#include "usbd_cdc_msc_hid.h"
#include "usbd_cdc_interface.h"
#include "pendsv.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "shared/runtime/interrupt_char.h"
#include "irq.h"

#if MICROPY_HW_ENABLE_USB

#if !MICROPY_HW_USB_IS_MULTI_OTG
#define USE_USB_CNTR_SOFM (1)
#if defined(STM32G0) || defined(STM32H5)
#define USB USB_DRD_FS
#endif
#else
#define USE_USB_CNTR_SOFM (0)
#endif

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

// Used to control the connect_state variable when USB host opens the serial port
static uint8_t usbd_cdc_connect_tx_timer;

#if MICROPY_HW_USB_CDC_1200BPS_TOUCH
static mp_sched_node_t mp_bootloader_sched_node;
static void usbd_cdc_run_bootloader_task(mp_sched_node_t *node) {
    mp_hal_delay_ms(250);
    machine_bootloader(0, NULL);
}
#endif

uint8_t *usbd_cdc_init(usbd_cdc_state_t *cdc_in) {
    usbd_cdc_itf_t *cdc = (usbd_cdc_itf_t *)cdc_in;

    // Reset the CDC state due to a new USB host connection
    // Note: we don't reset tx_buf_ptr_* in order to allow the output buffer to
    // be filled (by usbd_cdc_tx_always) before the USB device is connected, and
    // to retain transmit buffer state across multiple USB connections (they will
    // be 0 at MCU reset since the variables live in the BSS).
    cdc->rx_buf_put = 0;
    cdc->rx_buf_get = 0;
    cdc->rx_buf_full = false;
    cdc->tx_need_empty_packet = 0;
    cdc->connect_state = USBD_CDC_CONNECT_STATE_DISCONNECTED;
    if (cdc->attached_to_repl) {
        // Default behavior is non-blocking when attached to repl
        cdc->flow &= ~USBD_CDC_FLOWCONTROL_CTS;
    } else {
        cdc->flow |= USBD_CDC_FLOWCONTROL_CTS;
    }
    cdc->bitrate = 0;

    // Return the buffer to place the first USB OUT packet
    return cdc->rx_packet_buf;
}

void usbd_cdc_deinit(usbd_cdc_state_t *cdc_in) {
    usbd_cdc_itf_t *cdc = (usbd_cdc_itf_t *)cdc_in;
    cdc->connect_state = USBD_CDC_CONNECT_STATE_DISCONNECTED;
}

// Manage the CDC class requests
// cmd: command code
// pbuf: buffer containing command data (request parameters)
// length: number of data to be sent (in bytes)
// Returns USBD_OK if all operations are OK else USBD_FAIL
int8_t usbd_cdc_control(usbd_cdc_state_t *cdc_in, uint8_t cmd, uint8_t *pbuf, uint16_t length) {
    usbd_cdc_itf_t *cdc = (usbd_cdc_itf_t *)cdc_in;

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
            cdc->bitrate = *((uint32_t *)pbuf);
            break;

        case CDC_GET_LINE_CODING:
            pbuf[0] = (uint8_t)(cdc->bitrate);
            pbuf[1] = (uint8_t)(cdc->bitrate >> 8);
            pbuf[2] = (uint8_t)(cdc->bitrate >> 16);
            pbuf[3] = (uint8_t)(cdc->bitrate >> 24);
            pbuf[4] = 0; // stop bits (1)
            pbuf[5] = 0; // parity (none)
            pbuf[6] = 8; // number of bits (8)
            break;

        case CDC_SET_CONTROL_LINE_STATE: {
            // wValue, indicating the state, is passed in length (bit of a hack)
            if (length & 1) {
                // The actual connection state is delayed to give the host a chance to
                // configure its serial port (in most cases to disable local echo)
                cdc->connect_state = USBD_CDC_CONNECT_STATE_CONNECTING;
                usbd_cdc_connect_tx_timer = 8; // wait for 8 SOF IRQs
                #if USE_USB_CNTR_SOFM
                USB->CNTR |= USB_CNTR_SOFM;
                #else
                PCD_HandleTypeDef *hpcd = cdc->base.usbd->pdev->pData;
                hpcd->Instance->GINTMSK |= USB_OTG_GINTMSK_SOFM;
                #endif
            } else {
                cdc->connect_state = USBD_CDC_CONNECT_STATE_DISCONNECTED;
                #if MICROPY_HW_USB_CDC_1200BPS_TOUCH
                if (cdc->bitrate == 1200) {
                    // Delay bootloader jump to allow the USB stack to service endpoints.
                    mp_sched_schedule_node(&mp_bootloader_sched_node, usbd_cdc_run_bootloader_task);
                }
                #endif
            }
            break;
        }

        case CDC_SEND_BREAK:
            /* Add your code here */
            break;

        default:
            break;
    }

    return USBD_OK;
}

static inline uint16_t usbd_cdc_tx_buffer_mask(uint16_t val) {
    return val & (MICROPY_HW_USB_CDC_TX_DATA_SIZE - 1);
}

static inline uint16_t usbd_cdc_tx_buffer_size(usbd_cdc_itf_t *cdc) {
    return cdc->tx_buf_ptr_in - cdc->tx_buf_ptr_out;
}

static inline bool usbd_cdc_tx_buffer_empty(usbd_cdc_itf_t *cdc) {
    return cdc->tx_buf_ptr_out == cdc->tx_buf_ptr_in;
}

static inline bool usbd_cdc_tx_buffer_will_be_empty(usbd_cdc_itf_t *cdc) {
    return cdc->tx_buf_ptr_out_next == cdc->tx_buf_ptr_in;
}

static inline bool usbd_cdc_tx_buffer_full(usbd_cdc_itf_t *cdc) {
    return usbd_cdc_tx_buffer_size(cdc) == MICROPY_HW_USB_CDC_TX_DATA_SIZE;
}

static uint16_t usbd_cdc_tx_send_length(usbd_cdc_itf_t *cdc) {
    uint16_t to_end = MICROPY_HW_USB_CDC_TX_DATA_SIZE - usbd_cdc_tx_buffer_mask(cdc->tx_buf_ptr_out);
    return MIN(usbd_cdc_tx_buffer_size(cdc), to_end);
}

static void usbd_cdc_tx_buffer_put(usbd_cdc_itf_t *cdc, uint8_t data, bool check_overflow) {
    cdc->tx_buf[usbd_cdc_tx_buffer_mask(cdc->tx_buf_ptr_in)] = data;
    cdc->tx_buf_ptr_in++;
    if (check_overflow && usbd_cdc_tx_buffer_size(cdc) > MICROPY_HW_USB_CDC_TX_DATA_SIZE) {
        cdc->tx_buf_ptr_out++;
        cdc->tx_buf_ptr_out_next = cdc->tx_buf_ptr_out;
    }
}

static uint8_t *usbd_cdc_tx_buffer_getp(usbd_cdc_itf_t *cdc, uint16_t len) {
    cdc->tx_buf_ptr_out_next += len;
    return &cdc->tx_buf[usbd_cdc_tx_buffer_mask(cdc->tx_buf_ptr_out)];
}

// Called when the USB IN endpoint is ready to receive more data
// (cdc.base.tx_in_progress must be 0)
void usbd_cdc_tx_ready(usbd_cdc_state_t *cdc_in) {

    usbd_cdc_itf_t *cdc = (usbd_cdc_itf_t *)cdc_in;
    cdc->tx_buf_ptr_out = cdc->tx_buf_ptr_out_next;

    if (usbd_cdc_tx_buffer_empty(cdc) && !cdc->tx_need_empty_packet) {
        // No outstanding data to send
        return;
    }
    uint16_t len = usbd_cdc_tx_send_length(cdc);
    // Should always succeed because cdc.base.tx_in_progress==0
    USBD_CDC_TransmitPacket(&cdc->base, len, usbd_cdc_tx_buffer_getp(cdc, len));


    // According to the USB specification, a packet size of 64 bytes (CDC_DATA_FS_MAX_PACKET_SIZE)
    // gets held at the USB host until the next packet is sent.  This is because a
    // packet of maximum size is considered to be part of a longer chunk of data, and
    // the host waits for all data to arrive (ie, waits for a packet < max packet size).
    // To flush a packet of exactly max packet size, we need to send a zero-size packet.
    // See eg http://www.cypress.com/?id=4&rID=92719
    cdc->tx_need_empty_packet = (len > 0 && len % usbd_cdc_max_packet(cdc->base.usbd->pdev) == 0 && usbd_cdc_tx_buffer_will_be_empty(cdc));
}

// Attempt to queue data on the USB IN endpoint
static void usbd_cdc_try_tx(usbd_cdc_itf_t *cdc) {
    uint32_t basepri = raise_irq_pri(IRQ_PRI_OTG_FS);
    if (cdc == NULL || cdc->connect_state == USBD_CDC_CONNECT_STATE_DISCONNECTED) {
        // CDC device is not connected to a host, so we are unable to send any data
    } else if (cdc->base.tx_in_progress) {
        // USB driver will call callback when ready
    } else {
        usbd_cdc_tx_ready(&cdc->base);
    }
    restore_irq_pri(basepri);
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd) {
    if (usbd_cdc_connect_tx_timer > 0) {
        --usbd_cdc_connect_tx_timer;
    } else {
        usbd_cdc_msc_hid_state_t *usbd = ((USBD_HandleTypeDef *)hpcd->pData)->pClassData;
        #if USE_USB_CNTR_SOFM
        USB->CNTR &= ~USB_CNTR_SOFM;
        #else
        hpcd->Instance->GINTMSK &= ~USB_OTG_GINTMSK_SOFM;
        #endif
        for (int i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
            usbd_cdc_itf_t *cdc = (usbd_cdc_itf_t *)usbd->cdc[i];
            if (cdc->connect_state == USBD_CDC_CONNECT_STATE_CONNECTING) {
                cdc->connect_state = USBD_CDC_CONNECT_STATE_CONNECTED;
                usbd_cdc_try_tx(cdc);
            }
        }
    }
}

bool usbd_cdc_rx_buffer_full(usbd_cdc_itf_t *cdc) {
    int get = cdc->rx_buf_get, put = cdc->rx_buf_put;
    int remaining = (get - put) + (-((int)(get <= put)) & MICROPY_HW_USB_CDC_RX_DATA_SIZE);
    return remaining < CDC_DATA_MAX_PACKET_SIZE + 1;
}

void usbd_cdc_rx_check_resume(usbd_cdc_itf_t *cdc) {
    uint32_t irq_state = disable_irq();
    if (cdc->rx_buf_full) {
        if (!usbd_cdc_rx_buffer_full(cdc)) {
            cdc->rx_buf_full = false;
            enable_irq(irq_state);
            USBD_CDC_ReceivePacket(&cdc->base, cdc->rx_packet_buf);
            return;
        }
    }
    enable_irq(irq_state);
}

// Data received over USB OUT endpoint is processed here.
// len: number of bytes received into the buffer we passed to USBD_CDC_ReceivePacket
// Returns USBD_OK if all operations are OK else USBD_FAIL
int8_t usbd_cdc_receive(usbd_cdc_state_t *cdc_in, size_t len) {
    usbd_cdc_itf_t *cdc = (usbd_cdc_itf_t *)cdc_in;

    // copy the incoming data into the circular buffer
    for (const uint8_t *src = cdc->rx_packet_buf, *top = cdc->rx_packet_buf + len; src < top; ++src) {
        if (cdc->attached_to_repl && *src == mp_interrupt_char) {
            mp_sched_keyboard_interrupt();
        } else {
            uint16_t next_put = (cdc->rx_buf_put + 1) & (MICROPY_HW_USB_CDC_RX_DATA_SIZE - 1);
            if (next_put == cdc->rx_buf_get) {
                // overflow, we just discard the rest of the chars
                break;
            }
            cdc->rx_user_buf[cdc->rx_buf_put] = *src;
            cdc->rx_buf_put = next_put;
        }
    }

    MICROPY_BOARD_USBD_CDC_RX_EVENT(cdc);

    if ((cdc->flow & USBD_CDC_FLOWCONTROL_RTS) && (usbd_cdc_rx_buffer_full(cdc))) {
        cdc->rx_buf_full = true;
        return USBD_BUSY;
    } else {
        // initiate next USB packet transfer
        cdc->rx_buf_full = false;
        return USBD_CDC_ReceivePacket(&cdc->base, cdc->rx_packet_buf);
    }
}

int usbd_cdc_tx_half_empty(usbd_cdc_itf_t *cdc) {
    int32_t tx_waiting = usbd_cdc_tx_buffer_size(cdc);
    return tx_waiting <= MICROPY_HW_USB_CDC_TX_DATA_SIZE / 2;
}

// Writes only the data that fits if flow & CTS, else writes all data
// Returns number of bytes actually written to the device
int usbd_cdc_tx_flow(usbd_cdc_itf_t *cdc, const uint8_t *buf, uint32_t len) {
    if (cdc->flow & USBD_CDC_FLOWCONTROL_CTS) {
        // Only write as much as can fit in tx buffer
        return usbd_cdc_tx(cdc, buf, len, 0);
    } else {
        // Never block, keep most recent data in rolling buffer
        usbd_cdc_tx_always(cdc, buf, len);
        return len;
    }
}

// timeout in milliseconds.
// Returns number of bytes written to the device.
int usbd_cdc_tx(usbd_cdc_itf_t *cdc, const uint8_t *buf, uint32_t len, uint32_t timeout) {
    for (uint32_t i = 0; i < len; i++) {
        // Wait until the device is connected and the buffer has space, with a given timeout
        uint32_t start = HAL_GetTick();
        while (cdc->connect_state == USBD_CDC_CONNECT_STATE_DISCONNECTED || usbd_cdc_tx_buffer_full(cdc)) {
            usbd_cdc_try_tx(cdc);
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
        usbd_cdc_tx_buffer_put(cdc, buf[i], false);
    }

    usbd_cdc_try_tx(cdc);

    // Success, return number of bytes read
    return len;
}

// Always write all of the data to the device tx buffer, even if the
// device is not connected, or if the buffer is full.  Has a small timeout
// to wait for the buffer to be drained, in the case the device is connected.
void usbd_cdc_tx_always(usbd_cdc_itf_t *cdc, const uint8_t *buf, uint32_t len) {
    for (int i = 0; i < len; i++) {
        // If the CDC device is not connected to the host then we don't have anyone to receive our data.
        // The device may become connected in the future, so we should at least try to fill the buffer
        // and hope that it doesn't overflow by the time the device connects.
        // If the device is not connected then we should go ahead and fill the buffer straight away,
        // ignoring overflow.  Otherwise, we should make sure that we have enough room in the buffer.
        if (cdc->connect_state != USBD_CDC_CONNECT_STATE_DISCONNECTED) {
            // If the buffer is full, wait until it gets drained, with a timeout of 500ms
            // (wraparound of tick is taken care of by 2's complement arithmetic).
            uint32_t start = HAL_GetTick();
            while (usbd_cdc_tx_buffer_full(cdc) && HAL_GetTick() - start <= 500) {
                usbd_cdc_try_tx(cdc);
                if (cdc->base.usbd->pdev->dev_state == USBD_STATE_SUSPENDED) {
                    // The USB is suspended so buffer will never be drained; exit loop
                    break;
                }
                if (query_irq() == IRQ_STATE_DISABLED) {
                    // IRQs disabled so buffer will never be drained; exit loop
                    break;
                }
                __WFI(); // enter sleep mode, waiting for interrupt
            }
        }

        usbd_cdc_tx_buffer_put(cdc, buf[i], true);
    }
    usbd_cdc_try_tx(cdc);
}

// Returns number of bytes in the rx buffer.
int usbd_cdc_rx_num(usbd_cdc_itf_t *cdc) {
    int32_t rx_waiting = (int32_t)cdc->rx_buf_put - (int32_t)cdc->rx_buf_get;
    if (rx_waiting < 0) {
        rx_waiting += MICROPY_HW_USB_CDC_RX_DATA_SIZE;
    }
    usbd_cdc_rx_check_resume(cdc);
    return rx_waiting;
}

// timeout in milliseconds.
// Returns number of bytes read from the device.
int usbd_cdc_rx(usbd_cdc_itf_t *cdc, uint8_t *buf, uint32_t len, uint32_t timeout) {
    // loop to read bytes
    for (uint32_t i = 0; i < len; i++) {
        // Wait until we have at least 1 byte to read
        uint32_t start = HAL_GetTick();
        while (cdc->rx_buf_put == cdc->rx_buf_get) {
            // Wraparound of tick is taken care of by 2's complement arithmetic.
            if (HAL_GetTick() - start >= timeout) {
                // timeout
                return i;
            }
            if (query_irq() == IRQ_STATE_DISABLED) {
                // IRQs disabled so buffer will never be filled; return immediately
                return i;
            }
            usbd_cdc_rx_check_resume(cdc);
            __WFI(); // enter sleep mode, waiting for interrupt
        }

        // Copy byte from device to user buffer
        buf[i] = cdc->rx_user_buf[cdc->rx_buf_get];
        cdc->rx_buf_get = (cdc->rx_buf_get + 1) & (MICROPY_HW_USB_CDC_RX_DATA_SIZE - 1);
    }
    usbd_cdc_rx_check_resume(cdc);

    // Success, return number of bytes read
    return len;
}

#endif
