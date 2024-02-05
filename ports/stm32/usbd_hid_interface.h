/*
 * This file is part of the MicroPython project, http://micropython.org/
 */
#ifndef MICROPY_INCLUDED_STM32_USBD_HID_INTERFACE_H
#define MICROPY_INCLUDED_STM32_USBD_HID_INTERFACE_H

#include <stdint.h>
#include "usbd_cdc_msc_hid.h"

#define USBD_HID_REPORT_INVALID ((size_t)-1)

typedef struct _usbd_hid_itf_t {
    usbd_hid_state_t base; // state for the base HID layer

    volatile size_t report_in_len;
    uint8_t report_in_buf[HID_DATA_FS_MAX_PACKET_SIZE];
} usbd_hid_itf_t;

usbd_hid_itf_t *usbd_hid_get(void);

static inline int usbd_hid_rx_num(usbd_hid_itf_t *hid) {
    return hid->report_in_len != USBD_HID_REPORT_INVALID;
}

int usbd_hid_rx(usbd_hid_itf_t *hid, size_t len, uint8_t *buf, uint32_t timeout_ms);

#endif // MICROPY_INCLUDED_STM32_USBD_HID_INTERFACE_H
