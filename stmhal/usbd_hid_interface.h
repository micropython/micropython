/*
 * This file is part of the MicroPython project, http://micropython.org/
 */
#ifndef MICROPY_INCLUDED_STMHAL_USBD_HID_INTERFACE_H
#define MICROPY_INCLUDED_STMHAL_USBD_HID_INTERFACE_H

#include "usbd_cdc_msc_hid.h"

extern const USBD_HID_ItfTypeDef USBD_HID_fops;

int USBD_HID_RxNum(void);
int USBD_HID_Rx(USBD_HandleTypeDef *pdev, uint8_t *buf, uint32_t len, uint32_t timeout);

#endif // MICROPY_INCLUDED_STMHAL_USBD_HID_INTERFACE_H
