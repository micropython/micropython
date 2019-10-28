/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include STM32_HAL_H
#include "usbd_ioreq.h"
#include "usbd_cdc_msc_hid.h"

#if MICROPY_HW_ENABLE_USB

#define HEAD_DESC_SIZE (9)
#define MSC_CLASS_DESC_SIZE (9 + 7 + 7)
#define CDC_CLASS_DESC_SIZE (8 + 58)
#define HID_CLASS_DESC_SIZE (9 + 9 + 7 + 7)

#define MSC_TEMPLATE_MSC_DESC_OFFSET (9)
#define CDC_MSC_TEMPLATE_MSC_DESC_OFFSET (9)
#define CDC_MSC_TEMPLATE_CDC_DESC_OFFSET (40)
#define CDC2_TEMPLATE_CDC_DESC_OFFSET (9 + 8)
#define CDC2_TEMPLATE_CDC2_DESC_OFFSET (9 + (8 + 58) + 8)
#define CDC2_MSC_TEMPLATE_MSC_DESC_OFFSET (9)
#define CDC2_MSC_TEMPLATE_CDC_DESC_OFFSET (9 + 23 + 8)
#define CDC2_MSC_TEMPLATE_CDC2_DESC_OFFSET (9 + 23 + (8 + 58) + 8)
#define CDC3_TEMPLATE_CDC_DESC_OFFSET (9 + 8)
#define CDC3_TEMPLATE_CDC2_DESC_OFFSET (9 + (8 + 58) + 8)
#define CDC3_TEMPLATE_CDC3_DESC_OFFSET (9 + (8 + 58) + (8 + 58) + 8)
#define CDC3_MSC_TEMPLATE_MSC_DESC_OFFSET (9)
#define CDC3_MSC_TEMPLATE_CDC_DESC_OFFSET (9 + 23 + 8)
#define CDC3_MSC_TEMPLATE_CDC2_DESC_OFFSET (9 + 23 + (8 + 58) + 8)
#define CDC3_MSC_TEMPLATE_CDC3_DESC_OFFSET (9 + 23 + (8 + 58) + (8 + 58) + 8)
#define CDC_HID_TEMPLATE_CDC_DESC_OFFSET (49)
#define CDC_TEMPLATE_CDC_DESC_OFFSET (9)
#define CDC_DESC_OFFSET_INTR_INTERVAL (34)
#define CDC_DESC_OFFSET_OUT_MAX_PACKET_LO (48)
#define CDC_DESC_OFFSET_OUT_MAX_PACKET_HI (49)
#define CDC_DESC_OFFSET_IN_MAX_PACKET_LO (55)
#define CDC_DESC_OFFSET_IN_MAX_PACKET_HI (56)
#define HID_DESC_OFFSET_SUBCLASS (6)
#define HID_DESC_OFFSET_PROTOCOL (7)
#define HID_DESC_OFFSET_SUBDESC (9)
#define HID_DESC_OFFSET_REPORT_DESC_LEN (16)
#define HID_DESC_OFFSET_IN_EP (20)
#define HID_DESC_OFFSET_MAX_PACKET_LO (22)
#define HID_DESC_OFFSET_MAX_PACKET_HI (23)
#define HID_DESC_OFFSET_POLLING_INTERVAL (24)
#define HID_DESC_OFFSET_OUT_EP (27)
#define HID_DESC_OFFSET_MAX_PACKET_OUT_LO (29)
#define HID_DESC_OFFSET_MAX_PACKET_OUT_HI (30)
#define HID_DESC_OFFSET_POLLING_INTERVAL_OUT (31)
#define HID_SUBDESC_LEN (9)

#define CDC_IFACE_NUM_ALONE (0)
#define CDC_IFACE_NUM_WITH_MSC (1)
#define CDC2_IFACE_NUM_WITH_CDC (2)
#define CDC3_IFACE_NUM_WITH_CDC (4)
#define CDC2_IFACE_NUM_WITH_MSC (3)
#define CDC3_IFACE_NUM_WITH_MSC (5)
#define CDC_IFACE_NUM_WITH_HID (1)
#define MSC_IFACE_NUM_WITH_CDC (0)
#define HID_IFACE_NUM_WITH_CDC (0)
#define HID_IFACE_NUM_WITH_MSC (1)
#define HID_IFACE_NUM_WITH_CDC_MSC (3)
#define HID_IFACE_NUM_WITH_CDC2_MSC (5)
#define HID_IFACE_NUM_WITH_CDC3_MSC (7)

#define CDC_IN_EP(i) (0x83 + 2 * (i))
#define CDC_OUT_EP(i) (0x03 + 2 * (i))
#define CDC_CMD_EP(i) (0x82 + 2 * (i))

#define HID_IN_EP_WITH_CDC (0x81)
#define HID_OUT_EP_WITH_CDC (0x01)
#define HID_IN_EP_WITH_MSC (0x83)
#define HID_OUT_EP_WITH_MSC (0x03)
#define HID_IN_EP_WITH_CDC_MSC (0x84)
#define HID_OUT_EP_WITH_CDC_MSC (0x04)
#define HID_IN_EP_WITH_CDC2_MSC (0x86)
#define HID_OUT_EP_WITH_CDC2_MSC (0x06)
#define HID_IN_EP_WITH_CDC3_MSC (0x88)
#define HID_OUT_EP_WITH_CDC3_MSC (0x08)

#define USB_DESC_TYPE_ASSOCIATION (0x0b)

#define CDC_CMD_PACKET_SIZE         (8)  // Control Endpoint Packet size

#define BOT_GET_MAX_LUN         (0xfe)
#define BOT_RESET               (0xff)

#define HID_DESCRIPTOR_TYPE     (0x21)
#define HID_REPORT_DESC         (0x22)
#define HID_REQ_SET_PROTOCOL    (0x0b)
#define HID_REQ_GET_PROTOCOL    (0x03)
#define HID_REQ_SET_IDLE        (0x0a)
#define HID_REQ_GET_IDLE        (0x02)

// Value used in the configuration descriptor for the bmAttributes entry
#if MICROPY_HW_USB_SELF_POWERED
#define CONFIG_DESC_ATTRIBUTES (0xc0) // self powered
#else
#define CONFIG_DESC_ATTRIBUTES (0x80) // bus powered
#endif

// Value used in the configuration descriptor for the bMaxPower entry
#if defined(MICROPY_HW_USB_MAX_POWER_MA)
#define CONFIG_DESC_MAXPOWER (MICROPY_HW_USB_MAX_POWER_MA / 2) // in units of 2mA
#else
#define CONFIG_DESC_MAXPOWER (0xfa) // 500mA in units of 2mA
#endif

#if USBD_SUPPORT_HS_MODE
// USB Standard Device Descriptor
__ALIGN_BEGIN static uint8_t USBD_CDC_MSC_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
    USB_LEN_DEV_QUALIFIER_DESC,
    USB_DESC_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40, // same for CDC and MSC (latter being MSC_FS_MAX_PACKET), HID is 0x04
    0x01,
    0x00,
};
#endif

// USB partial configuration descriptor
static const uint8_t head_desc_data[HEAD_DESC_SIZE] = {
    //--------------------------------------------------------------------------
    // Configuration Descriptor
    0x09,   // bLength: Configuration Descriptor size
    USB_DESC_TYPE_CONFIGURATION, // bDescriptorType: Configuration
    0x00,   // wTotalLength -- to be filled in
    0x00,   // wTotalLength -- to be filled in
    0x00,   // bNumInterfaces -- to be filled in
    0x01,   // bConfigurationValue: Configuration value
    0x00,   // iConfiguration: Index of string descriptor describing the configuration
    CONFIG_DESC_ATTRIBUTES, // bmAttributes
    CONFIG_DESC_MAXPOWER, // bMaxPower
};

#if MICROPY_HW_USB_MSC
// USB MSC partial configuration descriptor
static const uint8_t msc_class_desc_data[MSC_CLASS_DESC_SIZE] = {
    //==========================================================================
    // MSC only has 1 interface so doesn't need an IAD

    //--------------------------------------------------------------------------
    // Interface Descriptor
    0x09,   // bLength: Interface Descriptor size
    USB_DESC_TYPE_INTERFACE, // bDescriptorType: interface descriptor
    MSC_IFACE_NUM_WITH_CDC, // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x02,   // bNumEndpoints
    0x08,   // bInterfaceClass: MSC Class
    0x06,   // bInterfaceSubClass : SCSI transparent
    0x50,   // nInterfaceProtocol
    0x00,   // iInterface:

    // Endpoint IN descriptor
    0x07,                           // bLength: Endpoint descriptor length
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint descriptor type
    MSC_IN_EP,                      // bEndpointAddress: IN, address 3
    0x02,                           // bmAttributes: Bulk endpoint type
    LOBYTE(MSC_FS_MAX_PACKET),      // wMaxPacketSize
    HIBYTE(MSC_FS_MAX_PACKET),
    0x00,                           // bInterval: ignore for Bulk transfer

    // Endpoint OUT descriptor
    0x07,                           // bLength: Endpoint descriptor length
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint descriptor type
    MSC_OUT_EP,                     // bEndpointAddress: OUT, address 3
    0x02,                           // bmAttributes: Bulk endpoint type
    LOBYTE(MSC_FS_MAX_PACKET),      // wMaxPacketSize
    HIBYTE(MSC_FS_MAX_PACKET),
    0x00,                           // bInterval: ignore for Bulk transfer
};
#endif

// USB CDC partial configuration descriptor
static const uint8_t cdc_class_desc_data[CDC_CLASS_DESC_SIZE] = {
    //==========================================================================
    // Interface Association for CDC VCP
    0x08,   // bLength: 8 bytes
    USB_DESC_TYPE_ASSOCIATION, // bDescriptorType: IAD
    0x00,   // bFirstInterface: first interface for this association -- to be filled in
    0x02,   // bInterfaceCount: nummber of interfaces for this association
    0x02,   // bFunctionClass: Communication Interface Class
    0x02,   // bFunctionSubClass: Abstract Control Model
    0x01,   // bFunctionProtocol: Common AT commands
    0x00,   // iFunction: index of string for this function

    //--------------------------------------------------------------------------
    // Interface Descriptor
    0x09,   // bLength: Interface Descriptor size
    USB_DESC_TYPE_INTERFACE, // bDescriptorType: Interface
    0x00,   // bInterfaceNumber: Number of Interface -- to be filled in
    0x00,   // bAlternateSetting: Alternate setting
    0x01,   // bNumEndpoints: One endpoints used
    0x02,   // bInterfaceClass: Communication Interface Class
    0x02,   // bInterfaceSubClass: Abstract Control Model
    0x01,   // bInterfaceProtocol: Common AT commands
    0x00,   // iInterface:

    // Header Functional Descriptor
    0x05,   // bLength: Endpoint Descriptor size
    0x24,   // bDescriptorType: CS_INTERFACE
    0x00,   // bDescriptorSubtype: Header Func Desc
    0x10,   // bcdCDC: spec release number
    0x01,   // ?

    // Call Management Functional Descriptor
    0x05,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x01,   // bDescriptorSubtype: Call Management Func Desc
    0x00,   // bmCapabilities: D0+D1
    0x00,   // bDataInterface -- to be filled in

    // ACM Functional Descriptor
    0x04,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x02,   // bDescriptorSubtype: Abstract Control Management desc
    0x02,   // bmCapabilities

    // Union Functional Descriptor
    0x05,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x06,   // bDescriptorSubtype: Union func desc
    0x00,   // bMasterInterface: Communication class interface -- to be filled in
    0x00,   // bSlaveInterface0: Data Class Interface -- to be filled in

    // Endpoint CMD Descriptor
    0x07,                           // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint
    CDC_CMD_EP(0),                  // bEndpointAddress
    0x03,                           // bmAttributes: Interrupt
    LOBYTE(CDC_CMD_PACKET_SIZE),    // wMaxPacketSize:
    HIBYTE(CDC_CMD_PACKET_SIZE),
    0x20,                           // bInterval: polling interval in frames of 1ms

    //--------------------------------------------------------------------------
    // Data class interface descriptor
    0x09,   // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_INTERFACE, // bDescriptorType: interface
    0x00,   // bInterfaceNumber: Number of Interface -- to be filled in
    0x00,   // bAlternateSetting: Alternate setting
    0x02,   // bNumEndpoints: Two endpoints used
    0x0A,   // bInterfaceClass: CDC
    0x00,   // bInterfaceSubClass: ?
    0x00,   // bInterfaceProtocol: ?
    0x00,   // iInterface:

    // Endpoint OUT Descriptor
    0x07,                               // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,             // bDescriptorType: Endpoint
    CDC_OUT_EP(0),                      // bEndpointAddress
    0x02,                               // bmAttributes: Bulk
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),// wMaxPacketSize:
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00,                               // bInterval: ignore for Bulk transfer

    // Endpoint IN Descriptor
    0x07,                               // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,             // bDescriptorType: Endpoint
    CDC_IN_EP(0),                       // bEndpointAddress
    0x02,                               // bmAttributes: Bulk
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),// wMaxPacketSize:
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00,                               // bInterval: ignore for Bulk transfer
};

#if MICROPY_HW_USB_HID
// USB HID partial configuration descriptor
static const uint8_t hid_class_desc_data[HID_CLASS_DESC_SIZE] = {
    //==========================================================================
    // HID only has 1 interface so doesn't need an IAD

    //--------------------------------------------------------------------------
    // Interface Descriptor
    0x09,   // bLength: Interface Descriptor size
    USB_DESC_TYPE_INTERFACE, // bDescriptorType: interface descriptor
    HID_IFACE_NUM_WITH_CDC, // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x02,   // bNumEndpoints
    0x03,   // bInterfaceClass: HID Class
    0x01,   // bInterfaceSubClass: 0=no sub class, 1=boot
    0x02,   // nInterfaceProtocol: 0=none, 1=keyboard, 2=mouse
    0x00,   // iInterface:

    // HID descriptor
    0x09,                   // bLength: HID Descriptor size
    HID_DESCRIPTOR_TYPE,    // bDescriptorType: HID
    0x11,                   // bcdHID: HID Class Spec release number
    0x01,
    0x00,                   // bCountryCode: Hardware target country
    0x01,                   // bNumDescriptors: Number of HID class descriptors to follow
    0x22,                   // bDescriptorType
    USBD_HID_MOUSE_REPORT_DESC_SIZE, // wItemLength: Total length of Report descriptor
    0x00,

    // Endpoint IN descriptor
    0x07,                           // bLength: Endpoint descriptor length
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint descriptor type
    HID_IN_EP_WITH_CDC,             // bEndpointAddress: IN
    0x03,                           // bmAttributes: Interrupt endpoint type
    LOBYTE(USBD_HID_MOUSE_MAX_PACKET), // wMaxPacketSize
    HIBYTE(USBD_HID_MOUSE_MAX_PACKET),
    0x08,                           // bInterval: Polling interval

    // Endpoint OUT descriptor
    0x07,                           // bLength: Endpoint descriptor length
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint descriptor type
    HID_OUT_EP_WITH_CDC,             // bEndpointAddress: OUT
    0x03,                           // bmAttributes: Interrupt endpoint type
    LOBYTE(USBD_HID_MOUSE_MAX_PACKET), // wMaxPacketSize
    HIBYTE(USBD_HID_MOUSE_MAX_PACKET),
    0x08,                           // bInterval: Polling interval
};

__ALIGN_BEGIN const uint8_t USBD_HID_MOUSE_ReportDesc[USBD_HID_MOUSE_REPORT_DESC_SIZE] __ALIGN_END = {
    0x05, 0x01,     // Usage Page (Generic Desktop),
    0x09, 0x02,     // Usage (Mouse),
    0xA1, 0x01,     // Collection (Application),
    0x09, 0x01,         // Usage (Pointer),
    0xA1, 0x00,         // Collection (Physical),
    0x05, 0x09,         // Usage Page (Buttons),
    0x19, 0x01,         // Usage Minimum (01),
    0x29, 0x03,         // Usage Maximum (03),
    0x15, 0x00,         // Logical Minimum (0),
    0x25, 0x01,         // Logical Maximum (1),
    0x95, 0x03,         // Report Count (3),
    0x75, 0x01,         // Report Size (1),
    0x81, 0x02,         // Input(Data, Variable, Absolute), -- 3 button bits
    0x95, 0x01,         // Report Count(1),
    0x75, 0x05,         // Report Size(5),
    0x81, 0x01,         // Input(Constant),                 -- 5 bit padding
    0x05, 0x01,         // Usage Page (Generic Desktop),
    0x09, 0x30,         // Usage (X),
    0x09, 0x31,         // Usage (Y),
    0x09, 0x38,         // Usage (Wheel),
    0x15, 0x81,         // Logical Minimum (-127),
    0x25, 0x7F,         // Logical Maximum (127),
    0x75, 0x08,         // Report Size (8),
    0x95, 0x03,         // Report Count (3),
    0x81, 0x06,         // Input(Data, Variable, Relative), -- 3 position bytes (X,Y,Wheel)
    0xC0,           // End Collection,
    0x09, 0x3c,     // Usage (Motion Wakeup),
    0x05, 0xff,     // Usage Page (?),
    0x09, 0x01,     // Usage (?),
    0x15, 0x00,     // Logical Minimum (0),
    0x25, 0x01,     // Logical Maximum (1),
    0x75, 0x01,     // Report Size(1),
    0x95, 0x02,     // Report Count(2),
    0xb1, 0x22,     // ?
    0x75, 0x06,     // Report Size(6),
    0x95, 0x01,     // Report Count(1),
    0xb1, 0x01,     // ?
    0xc0            // End Collection
};

__ALIGN_BEGIN const uint8_t USBD_HID_KEYBOARD_ReportDesc[USBD_HID_KEYBOARD_REPORT_DESC_SIZE] __ALIGN_END = {
    // From p69 of http://www.usb.org/developers/devclass_docs/HID1_11.pdf
    0x05, 0x01,     // Usage Page (Generic Desktop),
    0x09, 0x06,     // Usage (Keyboard),
    0xA1, 0x01,     // Collection (Application),
    0x05, 0x07,         // Usage Page (Key Codes);
    0x19, 0xE0,         // Usage Minimum (224),
    0x29, 0xE7,         // Usage Maximum (231),
    0x15, 0x00,         // Logical Minimum (0),
    0x25, 0x01,         // Logical Maximum (1),
    0x75, 0x01,         // Report Size (1),
    0x95, 0x08,         // Report Count (8),
    0x81, 0x02,         // Input (Data, Variable, Absolute), ;Modifier byte
    0x95, 0x01,         // Report Count (1),
    0x75, 0x08,         // Report Size (8),
    0x81, 0x01,         // Input (Constant), ;Reserved byte
    0x95, 0x05,         // Report Count (5),
    0x75, 0x01,         // Report Size (1),
    0x05, 0x08,         // Usage Page (Page# for LEDs),
    0x19, 0x01,         // Usage Minimum (1),
    0x29, 0x05,         // Usage Maximum (5),
    0x91, 0x02,         // Output (Data, Variable, Absolute), ;LED report
    0x95, 0x01,         // Report Count (1),
    0x75, 0x03,         // Report Size (3),
    0x91, 0x01,         // Output (Constant), ;LED report padding
    0x95, 0x06,         // Report Count (6),
    0x75, 0x08,         // Report Size (8),
    0x15, 0x00,         // Logical Minimum (0),
    0x25, 0x65,         // Logical Maximum(101),
    0x05, 0x07,         // Usage Page (Key Codes),
    0x19, 0x00,         // Usage Minimum (0),
    0x29, 0x65,         // Usage Maximum (101),
    0x81, 0x00,         // Input (Data, Array), ;Key arrays (6 bytes)
    0xC0            // End Collection
};
#endif

static void make_head_desc(uint8_t *dest, uint16_t len, uint8_t num_itf) {
    memcpy(dest, head_desc_data, sizeof(head_desc_data));
    dest[2] = LOBYTE(len); // wTotalLength
    dest[3] = HIBYTE(len);
    dest[4] = num_itf; // bNumInterfaces
}

#if MICROPY_HW_USB_MSC
static size_t make_msc_desc(uint8_t *dest) {
    memcpy(dest, msc_class_desc_data, sizeof(msc_class_desc_data));
    return sizeof(msc_class_desc_data);
}
#endif

static size_t make_cdc_desc(uint8_t *dest, int need_iad, uint8_t iface_num) {
    if (need_iad) {
        memcpy(dest, cdc_class_desc_data, sizeof(cdc_class_desc_data));
        dest[2] = iface_num; // bFirstInterface
        dest += 8;
    } else {
        memcpy(dest, cdc_class_desc_data + 8, sizeof(cdc_class_desc_data) - 8);
    }
    dest[2] = iface_num;        // bInterfaceNumber, main class
    dest[18] = iface_num + 1;   // bDataInterface
    dest[26] = iface_num + 0;   // bMasterInterface
    dest[27] = iface_num + 1;   // bSlaveInterface
    dest[37] = iface_num + 1;   // bInterfaceNumber, data class
    return need_iad ? 8 + 58 : 58;
}

#if MICROPY_HW_USB_CDC_NUM >= 2
static size_t make_cdc_desc_ep(uint8_t *dest, int need_iad, uint8_t iface_num, uint8_t cmd_ep, uint8_t out_ep, uint8_t in_ep) {
    size_t n = make_cdc_desc(dest, need_iad, iface_num);
    if (need_iad) {
        dest += 8;
    }
    dest[30] = cmd_ep;          // bEndpointAddress, main class CMD
    dest[46] = out_ep;          // bEndpointAddress, data class OUT
    dest[53] = in_ep;           // bEndpointAddress, data class IN
    return n;
}
#endif

#if MICROPY_HW_USB_HID
static size_t make_hid_desc(uint8_t *dest, USBD_HID_ModeInfoTypeDef *hid_info, uint8_t iface_num) {
    memcpy(dest, hid_class_desc_data, sizeof(hid_class_desc_data));
    dest[2] = iface_num;
    dest[HID_DESC_OFFSET_SUBCLASS] = hid_info->subclass;
    dest[HID_DESC_OFFSET_PROTOCOL] = hid_info->protocol;
    dest[HID_DESC_OFFSET_REPORT_DESC_LEN] = hid_info->report_desc_len;
    dest[HID_DESC_OFFSET_MAX_PACKET_LO] = hid_info->max_packet_len;
    dest[HID_DESC_OFFSET_MAX_PACKET_HI] = 0;
    dest[HID_DESC_OFFSET_POLLING_INTERVAL] = hid_info->polling_interval;
    dest[HID_DESC_OFFSET_MAX_PACKET_OUT_LO] = hid_info->max_packet_len;
    dest[HID_DESC_OFFSET_MAX_PACKET_OUT_HI] = 0;
    dest[HID_DESC_OFFSET_POLLING_INTERVAL_OUT] = hid_info->polling_interval;
    return sizeof(hid_class_desc_data);
}

#if MICROPY_HW_USB_MSC
static size_t make_hid_desc_ep(uint8_t *dest, USBD_HID_ModeInfoTypeDef *hid_info, uint8_t iface_num, uint8_t in_ep, uint8_t out_ep) {
    size_t n = make_hid_desc(dest, hid_info, iface_num);
    dest[HID_DESC_OFFSET_IN_EP] = in_ep;
    dest[HID_DESC_OFFSET_OUT_EP] = out_ep;
    return n;
}
#endif
#endif

// return the saved usb mode
uint8_t USBD_GetMode(usbd_cdc_msc_hid_state_t *usbd) {
    return usbd->usbd_mode;
}

int USBD_SelectMode(usbd_cdc_msc_hid_state_t *usbd, uint32_t mode, USBD_HID_ModeInfoTypeDef *hid_info, uint8_t max_endpoint) {
    // save mode
    usbd->usbd_mode = mode;

    // construct config desc
    size_t n = HEAD_DESC_SIZE;
    uint8_t *d = usbd->usbd_config_desc;
    uint8_t num_itf = 0;
    switch (usbd->usbd_mode & USBD_MODE_IFACE_MASK) {
        #if MICROPY_HW_USB_MSC
        case USBD_MODE_MSC:
            n += make_msc_desc(d + n);
            num_itf = 1;
            break;

        case USBD_MODE_CDC_MSC:
            n += make_msc_desc(d + n);
            n += make_cdc_desc(d + n, 1, CDC_IFACE_NUM_WITH_MSC);
            usbd->cdc[0]->iface_num = CDC_IFACE_NUM_WITH_MSC;
            num_itf = 3;
            break;

        #if MICROPY_HW_USB_HID
        case USBD_MODE_CDC_MSC_HID:
            n += make_msc_desc(d + n);
            n += make_cdc_desc(d + n, 1, CDC_IFACE_NUM_WITH_MSC);
            usbd->hid->desc = d + n;
            n += make_hid_desc_ep(d + n, hid_info, HID_IFACE_NUM_WITH_CDC_MSC, HID_IN_EP_WITH_CDC_MSC, HID_OUT_EP_WITH_CDC_MSC);
            usbd->cdc[0]->iface_num = CDC_IFACE_NUM_WITH_MSC;
            usbd->hid->in_ep = HID_IN_EP_WITH_CDC_MSC;
            usbd->hid->out_ep = HID_OUT_EP_WITH_CDC_MSC;
            usbd->hid->iface_num = HID_IFACE_NUM_WITH_CDC_MSC;
            usbd->hid->report_desc = hid_info->report_desc;
            num_itf = 4;
            break;
        #endif
        #endif

        #if MICROPY_HW_USB_CDC_NUM >= 2
        case USBD_MODE_CDC2: {
            n += make_cdc_desc(d + n, 1, CDC_IFACE_NUM_ALONE);
            n += make_cdc_desc_ep(d + n, 1, CDC2_IFACE_NUM_WITH_CDC, CDC_CMD_EP(1), CDC_OUT_EP(1), CDC_IN_EP(1));
            usbd->cdc[0]->iface_num = CDC_IFACE_NUM_ALONE;
            usbd->cdc[1]->iface_num = CDC2_IFACE_NUM_WITH_CDC;
            num_itf = 4;
            break;
        }

        #if MICROPY_HW_USB_MSC
        case USBD_MODE_CDC2_MSC: {
            n += make_msc_desc(d + n);
            n += make_cdc_desc(d + n, 1, CDC_IFACE_NUM_WITH_MSC);
            n += make_cdc_desc_ep(d + n, 1, CDC2_IFACE_NUM_WITH_MSC, CDC_CMD_EP(1), CDC_OUT_EP(1), CDC_IN_EP(1));
            usbd->cdc[0]->iface_num = CDC_IFACE_NUM_WITH_MSC;
            usbd->cdc[1]->iface_num = CDC2_IFACE_NUM_WITH_MSC;
            num_itf = 5;
            break;
        }

        case USBD_MODE_CDC2_MSC_HID: {
            n += make_msc_desc(d + n);
            n += make_cdc_desc(d + n, 1, CDC_IFACE_NUM_WITH_MSC);
            n += make_cdc_desc_ep(d + n, 1, CDC2_IFACE_NUM_WITH_MSC, CDC_CMD_EP(1), CDC_OUT_EP(1), CDC_IN_EP(1));
            usbd->hid->desc = d + n;
            n += make_hid_desc_ep(d + n, hid_info, HID_IFACE_NUM_WITH_CDC2_MSC, HID_IN_EP_WITH_CDC2_MSC, HID_OUT_EP_WITH_CDC2_MSC);
            usbd->cdc[0]->iface_num = CDC_IFACE_NUM_WITH_MSC;
            usbd->cdc[1]->iface_num = CDC2_IFACE_NUM_WITH_MSC;
            usbd->hid->in_ep = HID_IN_EP_WITH_CDC2_MSC;
            usbd->hid->out_ep = HID_OUT_EP_WITH_CDC2_MSC;
            usbd->hid->iface_num = HID_IFACE_NUM_WITH_CDC2_MSC;
            usbd->hid->report_desc = hid_info->report_desc;
            num_itf = 6;
            break;
        }
        #endif
        #endif

        #if MICROPY_HW_USB_CDC_NUM >= 3
        case USBD_MODE_CDC3: {
            n += make_cdc_desc(d + n, 1, CDC_IFACE_NUM_ALONE);
            n += make_cdc_desc_ep(d + n, 1, CDC2_IFACE_NUM_WITH_CDC, CDC_CMD_EP(1), CDC_OUT_EP(1), CDC_IN_EP(1));
            n += make_cdc_desc_ep(d + n, 1, CDC3_IFACE_NUM_WITH_CDC, CDC_CMD_EP(2), CDC_OUT_EP(2), CDC_IN_EP(2));
            usbd->cdc[0]->iface_num = CDC_IFACE_NUM_ALONE;
            usbd->cdc[1]->iface_num = CDC2_IFACE_NUM_WITH_CDC;
            usbd->cdc[2]->iface_num = CDC3_IFACE_NUM_WITH_CDC;
            num_itf = 6;
            break;
        }

        #if MICROPY_HW_USB_MSC
        case USBD_MODE_CDC3_MSC: {
            n += make_msc_desc(d + n);
            n += make_cdc_desc(d + n, 1, CDC_IFACE_NUM_WITH_MSC);
            n += make_cdc_desc_ep(d + n, 1, CDC2_IFACE_NUM_WITH_MSC, CDC_CMD_EP(1), CDC_OUT_EP(1), CDC_IN_EP(1));
            n += make_cdc_desc_ep(d + n, 1, CDC3_IFACE_NUM_WITH_MSC, CDC_CMD_EP(2), CDC_OUT_EP(2), CDC_IN_EP(2));
            usbd->cdc[0]->iface_num = CDC_IFACE_NUM_WITH_MSC;
            usbd->cdc[1]->iface_num = CDC2_IFACE_NUM_WITH_MSC;
            usbd->cdc[2]->iface_num = CDC3_IFACE_NUM_WITH_MSC;
            num_itf = 7;
            break;
        }

        case USBD_MODE_CDC3_MSC_HID: {
            n += make_msc_desc(d + n);
            n += make_cdc_desc(d + n, 1, CDC_IFACE_NUM_WITH_MSC);
            n += make_cdc_desc_ep(d + n, 1, CDC2_IFACE_NUM_WITH_MSC, CDC_CMD_EP(1), CDC_OUT_EP(1), CDC_IN_EP(1));
            n += make_cdc_desc_ep(d + n, 1, CDC3_IFACE_NUM_WITH_MSC, CDC_CMD_EP(2), CDC_OUT_EP(2), CDC_IN_EP(2));
            usbd->hid->desc = d + n;
            n += make_hid_desc_ep(d + n, hid_info, HID_IFACE_NUM_WITH_CDC3_MSC, HID_IN_EP_WITH_CDC3_MSC, HID_OUT_EP_WITH_CDC3_MSC);
            usbd->cdc[0]->iface_num = CDC_IFACE_NUM_WITH_MSC;
            usbd->cdc[1]->iface_num = CDC2_IFACE_NUM_WITH_MSC;
            usbd->cdc[2]->iface_num = CDC3_IFACE_NUM_WITH_MSC;
            usbd->hid->in_ep = HID_IN_EP_WITH_CDC3_MSC;
            usbd->hid->out_ep = HID_OUT_EP_WITH_CDC3_MSC;
            usbd->hid->iface_num = HID_IFACE_NUM_WITH_CDC3_MSC;
            usbd->hid->report_desc = hid_info->report_desc;
            num_itf = 8;
            break;
        }
        #endif
        #endif

        #if MICROPY_HW_USB_HID
        case USBD_MODE_CDC_HID:
            usbd->hid->desc = d + n;
            n += make_hid_desc(d + n, hid_info, HID_IFACE_NUM_WITH_CDC);
            n += make_cdc_desc(d + n, 1, CDC_IFACE_NUM_WITH_HID);
            usbd->cdc[0]->iface_num = CDC_IFACE_NUM_WITH_HID;
            usbd->hid->in_ep = HID_IN_EP_WITH_CDC;
            usbd->hid->out_ep = HID_OUT_EP_WITH_CDC;
            usbd->hid->iface_num = HID_IFACE_NUM_WITH_CDC;
            usbd->hid->report_desc = hid_info->report_desc;
            num_itf = 3;
            break;
        #endif

        case USBD_MODE_CDC:
            n += make_cdc_desc(d + n, 0, CDC_IFACE_NUM_ALONE);
            usbd->cdc[0]->iface_num = CDC_IFACE_NUM_ALONE;
            num_itf = 2;
            break;

            /*
            // not implemented
        case USBD_MODE_MSC_HID:
            hid_in_ep = HID_IN_EP_WITH_MSC;
            hid_out_ep = HID_OUT_EP_WITH_MSC;
            hid_iface_num = HID_IFACE_NUM_WITH_MSC;
            break;
            */

        default:
            // mode not supported
            return -1;
    }

    make_head_desc(d, n, num_itf);
    usbd->usbd_config_desc_size = n;

    for (int i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
        if (usbd->usbd_mode & USBD_MODE_IFACE_CDC(i)) {
            usbd->cdc[i]->in_ep = CDC_IN_EP(i);
            usbd->cdc[i]->out_ep = CDC_OUT_EP(i);
        }
    }

    // Verify that the endpoints that are used fit within the maximum number
    d = usbd->usbd_config_desc;
    const uint8_t *d_top = d + n;
    while (d < d_top) {
        if (d[0] == 7 && d[1] == USB_DESC_TYPE_ENDPOINT && (d[2] & 0x7f) > max_endpoint) {
            // Endpoint out of range of hardware
            return -1;
        }
        d += d[0];
    }

    return 0;
}

static void usbd_cdc_state_init(USBD_HandleTypeDef *pdev, usbd_cdc_msc_hid_state_t *usbd, usbd_cdc_state_t *cdc, uint8_t cmd_ep) {
    int mp = usbd_cdc_max_packet(pdev);

    // Open endpoints
    USBD_LL_OpenEP(pdev, cdc->in_ep, USBD_EP_TYPE_BULK, mp);
    USBD_LL_OpenEP(pdev, cdc->out_ep, USBD_EP_TYPE_BULK, mp);
    USBD_LL_OpenEP(pdev, cmd_ep, USBD_EP_TYPE_INTR, CDC_CMD_PACKET_SIZE);

    // Init state
    cdc->usbd = usbd;
    cdc->cur_request = 0xff;
    cdc->tx_in_progress = 0;

    // Init interface
    uint8_t *buf = usbd_cdc_init(cdc);

    // Prepare Out endpoint to receive next packet
    USBD_LL_PrepareReceive(pdev, cdc->out_ep, buf, mp);
}

static uint8_t USBD_CDC_MSC_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    #if !USBD_SUPPORT_HS_MODE
    if (pdev->dev_speed == USBD_SPEED_HIGH) {
        // can't handle high speed
        return 1;
    }
    #endif

    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;

    // CDC VCP component(s)
    for (int i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
        if (usbd->usbd_mode & USBD_MODE_IFACE_CDC(i)) {
            usbd_cdc_state_init(pdev, usbd, usbd->cdc[i], CDC_CMD_EP(i));
        }
    }

    #if MICROPY_HW_USB_MSC
    if (usbd->usbd_mode & USBD_MODE_IFACE_MSC) {
        // MSC component

        int mp = usbd_msc_max_packet(pdev);

        // Open EP OUT
        USBD_LL_OpenEP(pdev,
                       MSC_OUT_EP,
                       USBD_EP_TYPE_BULK,
                       mp);

        // Open EP IN
        USBD_LL_OpenEP(pdev,
                       MSC_IN_EP,
                       USBD_EP_TYPE_BULK,
                       mp);

        // Init the BOT layer
        MSC_BOT_Init(pdev);
    }
    #endif

    #if MICROPY_HW_USB_HID
    if (usbd->usbd_mode & USBD_MODE_IFACE_HID) {
        // HID component

        // get max packet lengths from descriptor
        uint16_t mps_in =
            usbd->hid->desc[HID_DESC_OFFSET_MAX_PACKET_LO]
            | (usbd->hid->desc[HID_DESC_OFFSET_MAX_PACKET_HI] << 8);
        uint16_t mps_out =
            usbd->hid->desc[HID_DESC_OFFSET_MAX_PACKET_OUT_LO]
            | (usbd->hid->desc[HID_DESC_OFFSET_MAX_PACKET_OUT_HI] << 8);

        // Open EP IN
        USBD_LL_OpenEP(pdev, usbd->hid->in_ep, USBD_EP_TYPE_INTR, mps_in);

        // Open EP OUT
        USBD_LL_OpenEP(pdev, usbd->hid->out_ep, USBD_EP_TYPE_INTR, mps_out);


        usbd->hid->usbd = usbd;
        uint8_t *buf = usbd_hid_init(usbd->hid);

        // Prepare Out endpoint to receive next packet
        USBD_LL_PrepareReceive(pdev, usbd->hid->out_ep, buf, mps_out);

        usbd->hid->state = HID_IDLE;
    }
    #endif

    return 0;
}

static uint8_t USBD_CDC_MSC_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;

    for (int i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
        if ((usbd->usbd_mode & USBD_MODE_IFACE_CDC(i)) && usbd->cdc[i]) {
            // CDC VCP component

            usbd_cdc_deinit(usbd->cdc[i]);

            // close endpoints
            USBD_LL_CloseEP(pdev, CDC_IN_EP(i));
            USBD_LL_CloseEP(pdev, CDC_OUT_EP(i));
            USBD_LL_CloseEP(pdev, CDC_CMD_EP(i));
        }
    }

    #if MICROPY_HW_USB_MSC
    if (usbd->usbd_mode & USBD_MODE_IFACE_MSC) {
        // MSC component

        // close endpoints
        USBD_LL_CloseEP(pdev, MSC_OUT_EP);
        USBD_LL_CloseEP(pdev, MSC_IN_EP);

        // DeInit the BOT layer
        MSC_BOT_DeInit(pdev);
    }
    #endif

    #if MICROPY_HW_USB_HID
    if (usbd->usbd_mode & USBD_MODE_IFACE_HID) {
        // HID component

        // close endpoints
        USBD_LL_CloseEP(pdev, usbd->hid->in_ep);
        USBD_LL_CloseEP(pdev, usbd->hid->out_ep);
    }
    #endif

    return 0;
}

static uint8_t USBD_CDC_MSC_HID_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {

    /*
    printf("SU: %x %x %x %x\n", req->bmRequest, req->bRequest, req->wValue, req->wIndex);

    This is what we get when MSC is IFACE=0 and CDC is IFACE=1,2:
        SU: 21 22 0 1 -- USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE; CDC_SET_CONTROL_LINE_STATE
        SU: 21 20 0 1 -- USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE; CDC_SET_LINE_CODING
        SU: a1 fe 0 0 -- 0x80 | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE; BOT_GET_MAX_LUN; 0; 0
        SU: 21 22 3 1 -- USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE; CDC_SET_CONTROL_LINE_STATE

    On a Mac OS X, with MSC then CDC:
        SU: a1 fe 0 0
        SU: 21 22 2 1
        SU: 21 22 3 1
        SU: 21 20 0 1
    */

    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;

    // Work out the recipient of the setup request
    uint8_t mode = usbd->usbd_mode;
    uint8_t recipient = 0;
    usbd_cdc_state_t *cdc = NULL;
    switch (req->bmRequest & USB_REQ_RECIPIENT_MASK) {
        case USB_REQ_RECIPIENT_INTERFACE: {
            uint16_t iface = req->wIndex;
            #if MICROPY_HW_USB_MSC
            if ((mode & USBD_MODE_IFACE_MSC) && iface == MSC_IFACE_NUM_WITH_CDC) {
                recipient = USBD_MODE_MSC;
            } else
            #endif
            #if MICROPY_HW_USB_HID
            if ((mode & USBD_MODE_IFACE_HID) && iface == usbd->hid->iface_num) {
                recipient = USBD_MODE_HID;
            } else
            #endif
            {
                for (int i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
                    if ((mode & USBD_MODE_IFACE_CDC(i)) && iface == usbd->cdc[i]->iface_num) {
                        recipient = USBD_MODE_CDC;
                        cdc = usbd->cdc[i];
                        break;
                    }
                }
            }
            break;
        }
        case USB_REQ_RECIPIENT_ENDPOINT: {
            uint8_t ep = req->wIndex & 0x7f;
            #if MICROPY_HW_USB_MSC
            if ((mode & USBD_MODE_IFACE_MSC) && ep == MSC_OUT_EP) {
                recipient = USBD_MODE_MSC;
            } else
            #endif
            #if MICROPY_HW_USB_HID
            if ((mode & USBD_MODE_IFACE_HID) && ep == usbd->hid->out_ep) {
                recipient = USBD_MODE_HID;
            } else
            #endif
            {
                for (int i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
                    if ((mode & USBD_MODE_IFACE_CDC(i)) && (ep == CDC_OUT_EP(i) || ep == (CDC_CMD_EP(i) & 0x7f))) {
                        recipient = USBD_MODE_CDC;
                        cdc = usbd->cdc[i];
                        break;
                    }
                }
            }
            break;
        }
    }

    // Fail the request if we didn't have a valid recipient
    if (recipient == 0) {
        USBD_CtlError(pdev, req);
        return USBD_FAIL;
    }

    switch (req->bmRequest & USB_REQ_TYPE_MASK) {

        // Class request
        case USB_REQ_TYPE_CLASS:
            if (recipient == USBD_MODE_CDC) {
                if (req->wLength) {
                    if (req->bmRequest & 0x80) {
                        // device-to-host request
                        usbd_cdc_control(cdc, req->bRequest, (uint8_t*)cdc->ctl_packet_buf, req->wLength);
                        USBD_CtlSendData(pdev, (uint8_t*)cdc->ctl_packet_buf, req->wLength);
                    } else {
                        // host-to-device request
                        cdc->cur_request = req->bRequest;
                        cdc->cur_length = req->wLength;
                        USBD_CtlPrepareRx(pdev, (uint8_t*)cdc->ctl_packet_buf, req->wLength);
                    }
                } else {
                    // Not a Data request
                    // Transfer the command to the interface layer
                    return usbd_cdc_control(cdc, req->bRequest, NULL, req->wValue);
                }
            }
            #if MICROPY_HW_USB_MSC
            if (recipient == USBD_MODE_MSC) {
                switch (req->bRequest) {
                    case BOT_GET_MAX_LUN:
                        if ((req->wValue  == 0) && (req->wLength == 1) && ((req->bmRequest & 0x80) == 0x80)) {
                            usbd->MSC_BOT_ClassData.max_lun = usbd->MSC_BOT_ClassData.bdev_ops->GetMaxLun();
                            USBD_CtlSendData(pdev, (uint8_t *)&usbd->MSC_BOT_ClassData.max_lun, 1);
                        } else {
                            USBD_CtlError(pdev, req);
                            return USBD_FAIL;
                        }
                        break;

                    case BOT_RESET:
                        if ((req->wValue == 0) && (req->wLength == 0) && ((req->bmRequest & 0x80) != 0x80)) {
                            MSC_BOT_Reset(pdev);
                        } else {
                            USBD_CtlError(pdev, req);
                            return USBD_FAIL;
                        }
                        break;

                    default:
                        USBD_CtlError(pdev, req);
                        return USBD_FAIL;
                }
            }
            #endif
            #if MICROPY_HW_USB_HID
            if (recipient == USBD_MODE_HID) {
                switch (req->bRequest) {
                    case HID_REQ_SET_PROTOCOL:
                        usbd->hid->ctl_protocol = (uint8_t)(req->wValue);
                        break;

                    case HID_REQ_GET_PROTOCOL:
                        USBD_CtlSendData(pdev, &usbd->hid->ctl_protocol, 1);
                        break;

                    case HID_REQ_SET_IDLE:
                        usbd->hid->ctl_idle_state = (uint8_t)(req->wValue >> 8);
                        break;

                    case HID_REQ_GET_IDLE:
                        USBD_CtlSendData(pdev, &usbd->hid->ctl_idle_state, 1);
                        break;

                    default:
                        USBD_CtlError(pdev, req);
                        return USBD_FAIL;
                }
            }
            #endif
            break;

        case USB_REQ_TYPE_STANDARD:
            #if MICROPY_HW_USB_MSC
            if (recipient == USBD_MODE_MSC) {
                switch (req->bRequest) {
                    case USB_REQ_GET_INTERFACE :
                        USBD_CtlSendData(pdev, (uint8_t *)&usbd->MSC_BOT_ClassData.interface, 1);
                        break;

                    case USB_REQ_SET_INTERFACE :
                        usbd->MSC_BOT_ClassData.interface = (uint8_t)(req->wValue);
                        break;

                    case USB_REQ_CLEAR_FEATURE:
                        // Flush the FIFO and Clear the stall status
                        USBD_LL_FlushEP(pdev, (uint8_t)req->wIndex);

                        // Re-activate the EP
                        USBD_LL_CloseEP(pdev, (uint8_t)req->wIndex);
                        if((((uint8_t)req->wIndex) & 0x80) == 0x80) {
                            // Open EP IN
                            USBD_LL_OpenEP(pdev, MSC_IN_EP, USBD_EP_TYPE_BULK, usbd_msc_max_packet(pdev));
                        } else {
                            // Open EP OUT
                            USBD_LL_OpenEP(pdev, MSC_OUT_EP, USBD_EP_TYPE_BULK, usbd_msc_max_packet(pdev));
                        }
                        // Handle BOT error
                        MSC_BOT_CplClrFeature(pdev, (uint8_t)req->wIndex);
                        break;
                }
            }
            #endif
            #if MICROPY_HW_USB_HID
            if (recipient == USBD_MODE_HID) {
                switch (req->bRequest) {
                    case USB_REQ_GET_DESCRIPTOR: {
                        uint16_t len = 0;
                        const uint8_t *pbuf = NULL;
                        if (req->wValue >> 8 == HID_REPORT_DESC) {
                            len = usbd->hid->desc[HID_DESC_OFFSET_REPORT_DESC_LEN];
                            len = MIN(len, req->wLength);
                            pbuf = usbd->hid->report_desc;
                        } else if (req->wValue >> 8 == HID_DESCRIPTOR_TYPE) {
                            len = MIN(HID_SUBDESC_LEN, req->wLength);
                            pbuf = usbd->hid->desc + HID_DESC_OFFSET_SUBDESC;
                        }
                        USBD_CtlSendData(pdev, (uint8_t*)pbuf, len);
                        break;
                    }

                    case USB_REQ_GET_INTERFACE:
                        USBD_CtlSendData(pdev, &usbd->hid->ctl_alt_setting, 1);
                        break;

                    case USB_REQ_SET_INTERFACE:
                        usbd->hid->ctl_alt_setting = (uint8_t)(req->wValue);
                        break;
                }
            }
            #endif
            break;
    }
    return USBD_OK;
}

/* unused
static uint8_t EP0_TxSent(USBD_HandleTypeDef *pdev) {
}
*/

static uint8_t USBD_CDC_MSC_HID_EP0_RxReady(USBD_HandleTypeDef *pdev) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;
    for (int i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
        if (usbd->cdc[i] != NULL && usbd->cdc[i]->cur_request != 0xff) {
            usbd_cdc_control(usbd->cdc[i], usbd->cdc[i]->cur_request, (uint8_t*)usbd->cdc[i]->ctl_packet_buf, usbd->cdc[i]->cur_length);
            usbd->cdc[i]->cur_request = 0xff;
        }
    }
    return USBD_OK;
}

static uint8_t USBD_CDC_MSC_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;

    for (int i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
        if ((usbd->usbd_mode & USBD_MODE_IFACE_CDC(i)) && (epnum == (CDC_IN_EP(i) & 0x7f) || epnum == (CDC_CMD_EP(i) & 0x7f))) {
            usbd->cdc[i]->tx_in_progress = 0;
            usbd_cdc_tx_ready(usbd->cdc[i]);
            return USBD_OK;
        }
    }

    #if MICROPY_HW_USB_MSC
    if ((usbd->usbd_mode & USBD_MODE_IFACE_MSC) && epnum == (MSC_IN_EP & 0x7f)) {
        MSC_BOT_DataIn(pdev, epnum);
        return USBD_OK;
    }
    #endif

    #if MICROPY_HW_USB_HID
    if ((usbd->usbd_mode & USBD_MODE_IFACE_HID) && epnum == (usbd->hid->in_ep & 0x7f)) {
        /* Ensure that the FIFO is empty before a new transfer, this condition could
        be caused by  a new transfer before the end of the previous transfer */
        usbd->hid->state = HID_IDLE;
        return USBD_OK;
    }
    #endif

    return USBD_OK;
}

static uint8_t USBD_CDC_MSC_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;

    for (int i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
        if ((usbd->usbd_mode & USBD_MODE_IFACE_CDC(i)) && epnum == (CDC_OUT_EP(i) & 0x7f)) {
            size_t len = USBD_LL_GetRxDataSize(pdev, epnum);
            // USB data will be immediately processed, and next USB traffic is NAKed until it's done
            return usbd_cdc_receive(usbd->cdc[i], len);
        }
    }

    #if MICROPY_HW_USB_MSC
    if ((usbd->usbd_mode & USBD_MODE_IFACE_MSC) && epnum == (MSC_OUT_EP & 0x7f)) {
        MSC_BOT_DataOut(pdev, epnum);
        return USBD_OK;
    }
    #endif

    #if MICROPY_HW_USB_HID
    if ((usbd->usbd_mode & USBD_MODE_IFACE_HID) && epnum == (usbd->hid->out_ep & 0x7f)) {
        size_t len = USBD_LL_GetRxDataSize(pdev, epnum);
        return usbd_hid_receive(usbd->hid, len);
    }
    #endif

    return USBD_OK;
}

#if USBD_SUPPORT_HS_MODE
static void usbd_cdc_desc_config_max_packet(USBD_HandleTypeDef *pdev, uint8_t *cdc_desc) {
    uint32_t mp = usbd_cdc_max_packet(pdev);
    cdc_desc[CDC_DESC_OFFSET_OUT_MAX_PACKET_LO] = LOBYTE(mp);
    cdc_desc[CDC_DESC_OFFSET_OUT_MAX_PACKET_HI] = HIBYTE(mp);
    cdc_desc[CDC_DESC_OFFSET_IN_MAX_PACKET_LO] = LOBYTE(mp);
    cdc_desc[CDC_DESC_OFFSET_IN_MAX_PACKET_HI] = HIBYTE(mp);
    uint8_t interval; // polling interval in frames of 1ms
    if (pdev->dev_speed == USBD_SPEED_HIGH) {
        interval = 0x09;
    } else {
        interval = 0x20;
    }
    cdc_desc[CDC_DESC_OFFSET_INTR_INTERVAL] = interval;
}
#endif

static uint8_t *USBD_CDC_MSC_HID_GetCfgDesc(USBD_HandleTypeDef *pdev, uint16_t *length) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;

    #if USBD_SUPPORT_HS_MODE
    uint8_t *cdc_desc[MICROPY_HW_USB_CDC_NUM] = {0};
    uint8_t *msc_desc = NULL;
    switch (usbd->usbd_mode & USBD_MODE_IFACE_MASK) {
        #if MICROPY_HW_USB_MSC
        case USBD_MODE_MSC:
            msc_desc = usbd->usbd_config_desc + MSC_TEMPLATE_MSC_DESC_OFFSET;
            break;

        case USBD_MODE_CDC_MSC:
            cdc_desc[0] = usbd->usbd_config_desc + CDC_MSC_TEMPLATE_CDC_DESC_OFFSET;
            msc_desc = usbd->usbd_config_desc + CDC_MSC_TEMPLATE_MSC_DESC_OFFSET;
            break;
        #endif

        #if MICROPY_HW_USB_CDC_NUM >= 2
        case USBD_MODE_CDC2:
            cdc_desc[0] = usbd->usbd_config_desc + CDC2_TEMPLATE_CDC_DESC_OFFSET;
            cdc_desc[1] = usbd->usbd_config_desc + CDC2_TEMPLATE_CDC2_DESC_OFFSET;
            break;

        #if MICROPY_HW_USB_MSC
        case USBD_MODE_CDC2_MSC:
            cdc_desc[0] = usbd->usbd_config_desc + CDC2_MSC_TEMPLATE_CDC_DESC_OFFSET;
            cdc_desc[1] = usbd->usbd_config_desc + CDC2_MSC_TEMPLATE_CDC2_DESC_OFFSET;
            msc_desc = usbd->usbd_config_desc + CDC2_MSC_TEMPLATE_MSC_DESC_OFFSET;
            break;
        #endif
        #endif

        #if MICROPY_HW_USB_CDC_NUM >= 3
        case USBD_MODE_CDC3:
            cdc_desc[0] = usbd->usbd_config_desc + CDC3_TEMPLATE_CDC_DESC_OFFSET;
            cdc_desc[1] = usbd->usbd_config_desc + CDC3_TEMPLATE_CDC2_DESC_OFFSET;
            cdc_desc[2] = usbd->usbd_config_desc + CDC3_TEMPLATE_CDC3_DESC_OFFSET;
            break;

        #if MICROPY_HW_USB_MSC
        case USBD_MODE_CDC3_MSC:
            cdc_desc[0] = usbd->usbd_config_desc + CDC3_MSC_TEMPLATE_CDC_DESC_OFFSET;
            cdc_desc[1] = usbd->usbd_config_desc + CDC3_MSC_TEMPLATE_CDC2_DESC_OFFSET;
            cdc_desc[2] = usbd->usbd_config_desc + CDC3_MSC_TEMPLATE_CDC3_DESC_OFFSET;
            msc_desc = usbd->usbd_config_desc + CDC3_MSC_TEMPLATE_MSC_DESC_OFFSET;
            break;
        #endif
        #endif

        #if MICROPY_HW_USB_HID
        case USBD_MODE_CDC_HID:
            cdc_desc[0] = usbd->usbd_config_desc + CDC_HID_TEMPLATE_CDC_DESC_OFFSET;
            break;
        #endif

        case USBD_MODE_CDC:
            cdc_desc[0] = usbd->usbd_config_desc + CDC_TEMPLATE_CDC_DESC_OFFSET;
            break;
    }

    // configure CDC descriptors, if needed
    for (int i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
        if (cdc_desc[i] != NULL) {
            usbd_cdc_desc_config_max_packet(pdev, cdc_desc[i]);
        }
    }

    if (msc_desc != NULL) {
        uint32_t mp = usbd_msc_max_packet(pdev);
        msc_desc[13] = LOBYTE(mp);
        msc_desc[14] = HIBYTE(mp);
        msc_desc[20] = LOBYTE(mp);
        msc_desc[21] = HIBYTE(mp);
    }
    #endif

    *length = usbd->usbd_config_desc_size;
    return usbd->usbd_config_desc;
}

uint8_t *USBD_CDC_MSC_HID_GetDeviceQualifierDescriptor(USBD_HandleTypeDef *pdev, uint16_t *length) {
    #if USBD_SUPPORT_HS_MODE
    *length = sizeof(USBD_CDC_MSC_HID_DeviceQualifierDesc);
    return USBD_CDC_MSC_HID_DeviceQualifierDesc;
    #else
    *length = 0;
    return NULL;
    #endif
}

// data received on non-control OUT endpoint
uint8_t USBD_CDC_TransmitPacket(usbd_cdc_state_t *cdc, size_t len, const uint8_t *buf) {
    if (cdc->tx_in_progress == 0) {
        // transmit next packet
        USBD_LL_Transmit(cdc->usbd->pdev, cdc->in_ep, (uint8_t*)buf, len);

        // Tx transfer in progress
        cdc->tx_in_progress = 1;
        return USBD_OK;
    } else {
        return USBD_BUSY;
    }
}

// prepare OUT endpoint for reception
uint8_t USBD_CDC_ReceivePacket(usbd_cdc_state_t *cdc, uint8_t *buf) {
    // Suspend or Resume USB Out process

    #if !USBD_SUPPORT_HS_MODE
    if (cdc->usbd->pdev->dev_speed == USBD_SPEED_HIGH) {
        return USBD_FAIL;
    }
    #endif

    // Prepare Out endpoint to receive next packet
    USBD_LL_PrepareReceive(cdc->usbd->pdev, cdc->out_ep, buf, usbd_cdc_max_packet(cdc->usbd->pdev));

    return USBD_OK;
}

#if MICROPY_HW_USB_HID

// prepare OUT endpoint for reception
uint8_t USBD_HID_ReceivePacket(usbd_hid_state_t *hid, uint8_t *buf) {
    // Suspend or Resume USB Out process

    #if !USBD_SUPPORT_HS_MODE
    if (hid->usbd->pdev->dev_speed == USBD_SPEED_HIGH) {
        return USBD_FAIL;
    }
    #endif

    // Prepare Out endpoint to receive next packet
    uint16_t mps_out =
        hid->desc[HID_DESC_OFFSET_MAX_PACKET_OUT_LO]
        | (hid->desc[HID_DESC_OFFSET_MAX_PACKET_OUT_HI] << 8);
    USBD_LL_PrepareReceive(hid->usbd->pdev, hid->out_ep, buf, mps_out);

    return USBD_OK;
}

int USBD_HID_CanSendReport(usbd_hid_state_t *hid) {
    return hid->usbd->pdev->dev_state == USBD_STATE_CONFIGURED && hid->state == HID_IDLE;
}

uint8_t USBD_HID_SendReport(usbd_hid_state_t *hid, uint8_t *report, uint16_t len) {
    if (hid->usbd->pdev->dev_state == USBD_STATE_CONFIGURED) {
        if (hid->state == HID_IDLE) {
            hid->state = HID_BUSY;
            USBD_LL_Transmit(hid->usbd->pdev, hid->in_ep, report, len);
            return USBD_OK;
        }
    }
    return USBD_FAIL;
}

#endif

// CDC/MSC/HID interface class callback structure
const USBD_ClassTypeDef USBD_CDC_MSC_HID = {
    USBD_CDC_MSC_HID_Init,
    USBD_CDC_MSC_HID_DeInit,
    USBD_CDC_MSC_HID_Setup,
    NULL, // EP0_TxSent
    USBD_CDC_MSC_HID_EP0_RxReady,
    USBD_CDC_MSC_HID_DataIn,
    USBD_CDC_MSC_HID_DataOut,
    NULL, // SOF
    NULL, // IsoINIncomplete
    NULL, // IsoOUTIncomplete
    USBD_CDC_MSC_HID_GetCfgDesc,
    USBD_CDC_MSC_HID_GetCfgDesc,
    USBD_CDC_MSC_HID_GetCfgDesc,
    USBD_CDC_MSC_HID_GetDeviceQualifierDescriptor,
};

#endif
