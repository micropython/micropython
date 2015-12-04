/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "py/obj.h"
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

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

// rx/tx buffer sizes depend on the following
// 1) interval that the USB host polls the device (unreliable, specified in the descriptor).
//    64 bytes / polling interval gives the total number of bytes per second
//    e.g. if polling interval = 10msec -> 64 / 0.01 = 6.4kB/sec. the minimum polling interval in the
//    USB descriptor is 1msec - hence a theoretical maximum of 64kB /sec (or 512kBaud) is possible
// 2) you may increase the size also if the application wants to send big block of data without
//    being blocked by the timeout
#define APP_RX_DATA_SIZE  1024
#define APP_TX_DATA_SIZE  1024

void CDC_Itf_TxFinished(void);
void CDC_send_packet(void);

static __IO uint8_t dev_is_connected = 0; // indicates if we are connected
uint8_t UserRxBuffer[64];/* Received Data over USB are stored in this buffer */
uint8_t tx_buffer[APP_RX_DATA_SIZE];
uint8_t rx_buffer[APP_TX_DATA_SIZE];
bool CDC_is_busy;

typedef struct {
    uint8_t* start;
    uint8_t* end;
    uint16_t sizeof_data;
    uint8_t* push_ptr;
    uint8_t* pop_ptr;
} ringbuffer_t;

ringbuffer_t tx_ringbuffer;     // tx ringbuffer for USB CDC
ringbuffer_t rx_ringbuffer;     // rx ringbuffer for USB CDC

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

// static uint8_t UserTxNeedEmptyPacket = 0; // used to flush the USB IN endpoint if the last packet was exactly the endpoint packet size

static int user_interrupt_char = -1;
static void *user_interrupt_data = NULL;

/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_Itf_Init     (void);
static int8_t CDC_Itf_DeInit   (void);
static int8_t CDC_Itf_Control  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Itf_Receive  (uint8_t* pbuf, uint32_t *Len);

// CDC_Itf_TxFinished callback has been added in addition to the standard USB stack from ST
const USBD_CDC_ItfTypeDef USBD_CDC_fops = {
    CDC_Itf_Init,
    CDC_Itf_DeInit,
    CDC_Itf_Control,
    CDC_Itf_Receive,
    CDC_Itf_TxFinished
};

/* Private functions ---------------------------------------------------------*/

/**
 * simple ringbuffer handling that support putc/getc/is_full/is_empty functionality
 * may further be optmized by using 2'er complement masking instead of checking wraparound
 * conditions using if's (but the improvement is minimal).
 *
 * Example usage:
 *
 * uint8_t tx_buffer[256];
 * uint8_t* buf;
 * ringbuffer_t tx_ringbuffer;
 *
 * // initialize buffer first using "ringbuffer_init"
 * ringbuffer_init(&tx_ringbuffer, tx_buffer, sizeof(tx_buffer));
 * ringbuffer_putc(&tx_ringbuffer, "H")
 * ringbuffer_putc(&tx_ringbuffer, "i")
 * ringbuffer_getc(&tx_ringbuffer, buf);
 * ...
 */

inline bool ringbuffer_is_empty(ringbuffer_t* rbuffer) {
    return (rbuffer->push_ptr == rbuffer->pop_ptr);
}

void ringbuffer_init(ringbuffer_t* rbuffer, uint8_t* user_data, uint16_t len) {
    rbuffer->start = rbuffer->push_ptr = rbuffer->pop_ptr = user_data;
    rbuffer->sizeof_data = len;
    rbuffer->end = rbuffer->start + len - 1;
}

uint16_t ringbuffer_get_used_mem(ringbuffer_t* rbuffer) {
    if (rbuffer->push_ptr > rbuffer->pop_ptr) {
        return rbuffer->push_ptr - rbuffer->pop_ptr;
    } else {
        return rbuffer->sizeof_data  - (rbuffer->pop_ptr - rbuffer->push_ptr);
    }
}

uint16_t ringbuffer_get_free_mem(ringbuffer_t* rbuffer) {
    if (rbuffer->push_ptr > rbuffer->pop_ptr) {
        return rbuffer->sizeof_data - (rbuffer->push_ptr - rbuffer->pop_ptr) - 1;
    } else {
        return rbuffer->pop_ptr - rbuffer->push_ptr - 1;
    }
}

bool ringbuffer_is_full(ringbuffer_t* rbuffer) {
    return !ringbuffer_get_free_mem(rbuffer);
}

bool ringbuffer_putc(ringbuffer_t* rbuffer, uint8_t character) {
    *rbuffer->push_ptr++ = character;
    if (rbuffer->push_ptr > rbuffer->end ) {
        rbuffer->push_ptr = rbuffer->start;
    }
    // check if buffer is full
    if (rbuffer->push_ptr == rbuffer->pop_ptr) {
        // move pop pointer one forward - overwrites oldest data
        rbuffer->pop_ptr++;
        if (rbuffer->pop_ptr  > rbuffer->end ) {
            rbuffer->pop_ptr = rbuffer->start;
        }
        return false;
    }
    return true;
}

bool ringbuffer_getc(ringbuffer_t* rbuffer, uint8_t* ch) {
    if (ringbuffer_is_empty(rbuffer)) {
        return false;
    }
    *ch = *(rbuffer->pop_ptr);
    rbuffer->pop_ptr++;
    if (rbuffer->pop_ptr  > rbuffer->end ) {
        // wraparound
        rbuffer->pop_ptr = rbuffer->start;
    }
    return true;
}
/**
  * @brief  CDC_Itf_Init
  *         Initializes the CDC media low layer
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Init(void)
{
#if 0
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* USART configured as follow:
      - Word Length = 8 Bits
      - Stop Bit    = One Stop bit
      - Parity      = No parity
      - BaudRate    = 115200 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance        = USARTx;
  UartHandle.Init.BaudRate   = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /*##-2- Put UART peripheral in IT reception process ########################*/
  /* Any data received will be stored in "UserTxBuffer" buffer  */
  if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)UserTxBuffer, 1) != HAL_OK)
  {
    /* Transfer error in reception process */
    Error_Handler();
  }
  
  /*##-3- Configure the TIM Base generation  #################################*/
  now done in HAL_MspInit
  TIM_Config();
#endif

    CDC_is_busy = false;

    ringbuffer_init(&tx_ringbuffer, tx_buffer, APP_TX_DATA_SIZE);
    ringbuffer_init(&rx_ringbuffer, rx_buffer, APP_RX_DATA_SIZE);
    /*##-5- Set Application Buffers ############################################*/
    USBD_CDC_SetTxBuffer(&hUSBDDevice, tx_ringbuffer.start, 0);
    USBD_CDC_SetRxBuffer(&hUSBDDevice, UserRxBuffer);

    /* NOTE: we cannot reset these here, because USBD_CDC_SetInterrupt
     * may be called before this init function to set these values.
     * This can happen if the USB enumeration occurs after the call to
     * USBD_CDC_SetInterrupt.
    user_interrupt_char = -1;
    user_interrupt_data = NULL;
    */

    return (USBD_OK);
}

/**
  * @brief  CDC_Itf_DeInit
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_DeInit(void)
{
#if 0
  /* DeInitialize the UART peripheral */
  if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
  }
#endif
  return (USBD_OK);
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
            #if 0
            pbuf[0] = (uint8_t)(LineCoding.bitrate);
            pbuf[1] = (uint8_t)(LineCoding.bitrate >> 8);
            pbuf[2] = (uint8_t)(LineCoding.bitrate >> 16);
            pbuf[3] = (uint8_t)(LineCoding.bitrate >> 24);
            pbuf[4] = LineCoding.format;
            pbuf[5] = LineCoding.paritytype;
            pbuf[6] = LineCoding.datatype;
            #endif

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

/**
  * @brief  CDC_Itf_TxFinished
  *         Callback of USB stack if a transmission to the USB host has finished
  *         Ringbuffer is checked and a new transmission initiated if it is not empty
  *         CDC_is_busy is used to indicate the transmission state
  */
void CDC_Itf_TxFinished(void) {
    if (ringbuffer_is_empty(&tx_ringbuffer)) {
        CDC_is_busy = false;
        return;
    }
    CDC_is_busy = true;
    CDC_send_packet();
}

/**
  * @brief  CDC_Itf_Receive
  *         Data received over USB OUT endpoint is processed here.
  * @param  Buf: Buffer of data received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK
  */
static int8_t CDC_Itf_Receive(uint8_t* Buf, uint32_t *Len) {
#if 0
    // this sends the data over the UART using DMA
    HAL_UART_Transmit_DMA(&UartHandle, Buf, *Len);
#endif

    uint32_t i=0;
    // copy over to ringbuffer, check for special interrupt char
    while (i < *Len) {
        if (Buf[i] == user_interrupt_char) {
            pendsv_nlr_jump(user_interrupt_data);
        } else {
            ringbuffer_putc(&rx_ringbuffer,Buf[i]);
        }
        i++;
    }
    USBD_CDC_SetRxBuffer(&hUSBDDevice, UserRxBuffer);
    USBD_CDC_ReceivePacket(&hUSBDDevice);

    return USBD_OK;
}

int USBD_CDC_IsConnected(void) {
    return dev_is_connected;
}

void USBD_CDC_SetInterrupt(int chr, void *data) {
    user_interrupt_char = chr;
    user_interrupt_data = data;
}
/**
  * @brief  poll_ringbuffer
  *         polls a ringbuffer until is is no more full or timeout
  * @param  timout      timeout in msec
  * @retval false on timout
  */
bool poll_ringbuffer(ringbuffer_t* rbuffer, int16_t timeout) {
    uint32_t start = HAL_GetTick();

    while (HAL_GetTick() - start <= timeout) {
        if (query_irq() == IRQ_STATE_DISABLED) {
            // IRQs disabled so buffer will never be filled; return immediately
            return true;
        }
        __WFI(); // enter sleep mode, waiting for interrupt (systick?)

        if (!ringbuffer_is_full(rbuffer)) {
            return true;
        }
    }
    // timeout
    return false;
}
/**
  * @brief  USBD_CDC_Tx_Abort
  *         Transmits a data buffer over USB CDC channel using a ringbuffer structure.
  *         if the ringbuffer is full, this function blocks for up to ::timeout msec for the buffer to be
  *         drained by the USB stack. After timeout it will either return the bytes transferred to the
  *         ringbuffer or it will silently overwrite the oldest data in the ringbuffer (abort_if_timeout = false)
  * @param  buf         pointer to the data to be transferred
  * @param  len         number of bytes
  * @param  timout      timeout in msec
  * @param  abort_if_timeout abort filling buffer if it is full
  * @retval number of bytes that were put in the ringbuffer
  */
int USBD_CDC_Tx_Abort(const uint8_t *buf, uint32_t len, uint32_t timeout, bool abort_if_timeout) {
    bool usb_is_transmitting = true;

    // ringbuffer_put_mem(&tx_ringbuffer, (uint8_t*)buf, len);
    for (uint32_t i = 0; i < len; i++) {
        if (dev_is_connected && usb_is_transmitting) {
            // store it, don't force store if buffer is full
            if (!ringbuffer_putc(&tx_ringbuffer, *buf++)) {
                // buffer is full
                // the handling is as follows:
                // 1) the ringbuffer state is polled for max 500msec. If the buffer is still full then it will
                //    exit and overwrite the oldest data in ringbuffer if ::abort_if_timeout is not set
                // 2) if the device continues to send (at mininum 1 character) then the timout is re-set again
                //    to 500msec. This can be modified if needed so that the start time is not reset
                usb_is_transmitting = poll_ringbuffer(&tx_ringbuffer, timeout);
                if ((!usb_is_transmitting) && abort_if_timeout) {
                    // abort transmission here
                    return i;
                }
            }
        } else {
            // either not connected or timout - overwrite oldest ringbuffer data
            ringbuffer_putc(&tx_ringbuffer, *buf++);
        }
    }
    if (!CDC_is_busy && !ringbuffer_is_empty(&tx_ringbuffer)) {
        CDC_send_packet();
        CDC_is_busy = true;
    }
    return len;
}

int USBD_CDC_TxHalfEmpty(void) {
    return ringbuffer_get_free_mem(&tx_ringbuffer) > (tx_ringbuffer.sizeof_data / 2);
}

/**
  * @brief  USBD_CDC_TxAlways
  *         mphal callback - sends USB data with 500msec fixed timeout
  *         always puts data in buffer even if buffer is full (will overwrite oldest data in buffer in
  *         case of a timeout)
  * @param  buf         pointer to the data to be transferred
  * @param  len         number of bytes
  * @retval number of bytes that were put in the ringbuffer
  */
void USBD_CDC_TxAlways(const uint8_t *buf, uint32_t len) {
    USBD_CDC_Tx_Abort(buf,len,500,false);
}

/**
  * @brief  USBD_CDC_Tx
  *         mphal callback - sends USB data with a timeout option
  *         aborts transmission if buffer is full
  * @param  buf         pointer to the data to be transferred
  * @param  len         number of bytes
  * @param  timout      timeout in msec
  * @retval number of bytes that were put in the ringbuffer
  */
int USBD_CDC_Tx(const uint8_t *buf, uint32_t len, uint32_t timeout) {
    return USBD_CDC_Tx_Abort(buf,len,timeout,true);
}

// Returns number of bytes in the rx buffer.
int USBD_CDC_RxNum(void) {
    return ringbuffer_get_free_mem(&rx_ringbuffer);
}

/**
  * @brief  CDC_send_packet
  *         helper function, transfers a continous memory block over CDC
  *         checks for wraparound conditions
  */
void CDC_send_packet(void) {
    if (tx_ringbuffer.push_ptr < tx_ringbuffer.pop_ptr) {
        // wraparound
        USBD_CDC_SetTxBuffer(&hUSBDDevice, (uint8_t*)(tx_ringbuffer.pop_ptr), tx_ringbuffer.end - tx_ringbuffer.pop_ptr + 1 );
        tx_ringbuffer.pop_ptr = tx_ringbuffer.start;
    } else {
        USBD_CDC_SetTxBuffer(&hUSBDDevice, (uint8_t*)(tx_ringbuffer.pop_ptr), tx_ringbuffer.push_ptr - tx_ringbuffer.pop_ptr );
        tx_ringbuffer.pop_ptr = tx_ringbuffer.push_ptr;
    }
    USBD_CDC_TransmitPacket(&hUSBDDevice);

}
/**
  * @brief  USBD_CDC_Rx
  *         reads ::len number of bytes from the rx ringbuffer to the ::buf data pointer
  *         with a ::timout option in msec. returns the number of bytes returned from the
  *         ringbuffer
  */
int USBD_CDC_Rx(uint8_t *buf, uint32_t len, uint32_t timeout) {
    uint32_t i;
    for (i=0; i<len; i++) {
        // Wait until we have at least 1 byte to read
        if (!ringbuffer_getc(&rx_ringbuffer, buf++)) {
            // buffer is empty, wait
            uint32_t start;
            if (query_irq() == IRQ_STATE_DISABLED) {
                return i;
            }
            start = HAL_GetTick();
            while (ringbuffer_is_empty(&rx_ringbuffer)) {
                if (HAL_GetTick() - start <= timeout) {
                    // timeout - return the nr of bytes read so far
                    return i;
                }
                // sleep
                __WFI();
            }
        }
    }
    // Success, return number of bytes read
    return i;
}
