/*
 * This file is part of the MicroPython project, http://micropython.org/
 */
#ifndef MICROPY_INCLUDED_STM32_USBD_HID_INTERFACE_H
#define MICROPY_INCLUDED_STM32_USBD_HID_INTERFACE_H

#include "usbd_cdc_msc_hid.h"

typedef struct _usbd_hid_itf_t {
    usbd_cdc_msc_hid_state_t *usbd; // the parent USB device

    uint8_t buffer[2][HID_DATA_FS_MAX_PACKET_SIZE]; // pair of buffers to read individual packets into
    int8_t current_read_buffer; // which buffer to read from
    uint32_t last_read_len; // length of last read
    int8_t current_write_buffer; // which buffer to write to
} usbd_hid_itf_t;

int usbd_hid_rx_num(usbd_hid_itf_t *hid);
int usbd_hid_rx(usbd_hid_itf_t *hid, size_t len, uint8_t *buf, uint32_t timeout);

#endif // MICROPY_INCLUDED_STM32_USBD_HID_INTERFACE_H
