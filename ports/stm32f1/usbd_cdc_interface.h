/*
 * This file is part of the MicroPython project, http://micropython.org/
 */
#ifndef MICROPY_INCLUDED_STM32_USBD_CDC_INTERFACE_H
#define MICROPY_INCLUDED_STM32_USBD_CDC_INTERFACE_H

#define USBD_CDC_RX_DATA_SIZE (128) // this must be 2 or greater, and a power of 2
#define USBD_CDC_TX_DATA_SIZE (256) // I think this can be any value (was 2048)

// Values for connect_state - 连接状态定义
#define USBD_CDC_CONNECT_STATE_DISCONNECTED (0)
#define USBD_CDC_CONNECT_STATE_CONNECTING (1)
#define USBD_CDC_CONNECT_STATE_CONNECTED (2)

// Flow control settings - 流控定义
#define USBD_CDC_FLOWCONTROL_NONE (0)
#define USBD_CDC_FLOWCONTROL_RTS (1)

typedef struct _usbd_cdc_itf_t {
    usbd_cdc_state_t base; // state for the base CDC layer

    uint8_t           rx_packet_buf[CDC_DATA_MAX_PACKET_SIZE]; // received data from USB OUT endpoint is stored in this buffer
    uint8_t           rx_user_buf[USBD_CDC_RX_DATA_SIZE];      // received data is buffered here until the user reads it
    volatile uint16_t rx_buf_put;                              // 环形缓冲索引
    uint16_t          rx_buf_get;                              // 环形缓冲索引
    uint8_t           rx_buf_full;                             // rx from host will be blocked while this is true
    uint8_t           tx_buf[USBD_CDC_TX_DATA_SIZE];           // data for USB IN endpoind is stored in this buffer
    uint16_t          tx_buf_ptr_in;                           // increment this pointer modulo USBD_CDC_TX_DATA_SIZE when new data is available
    volatile uint16_t tx_buf_ptr_out;                          // increment this pointer modulo USBD_CDC_TX_DATA_SIZE when data is drained
    uint16_t          tx_buf_ptr_out_shadow;                   // shadow of above
    uint8_t           tx_need_empty_packet;                    // used to flush the USB IN endpoint if the last packet was exactly the endpoint packet size
    volatile uint8_t  connect_state;                           // indicates if we are connected
    uint8_t           attached_to_repl;                        // indicates if interface is connected to REPL
    uint8_t           flow;                                    // USBD_CDC_FLOWCONTROL_* setting flags
} usbd_cdc_itf_t;

// This is implemented in usb.c
usbd_cdc_itf_t *usb_vcp_get(int idx);

static inline int usbd_cdc_is_connected(usbd_cdc_itf_t *cdc) {
    return cdc->connect_state == USBD_CDC_CONNECT_STATE_CONNECTED;
}

int  usbd_cdc_tx_half_empty(usbd_cdc_itf_t *cdc);
int  usbd_cdc_tx           (usbd_cdc_itf_t *cdc, const uint8_t *buf, uint32_t len, uint32_t timeout);
void usbd_cdc_tx_always    (usbd_cdc_itf_t *cdc, const uint8_t *buf, uint32_t len);

int usbd_cdc_rx_num(usbd_cdc_itf_t *cdc);
int usbd_cdc_rx    (usbd_cdc_itf_t *cdc, uint8_t *buf, uint32_t len, uint32_t timeout);

#endif // MICROPY_INCLUDED_STM32_USBD_CDC_INTERFACE_H
