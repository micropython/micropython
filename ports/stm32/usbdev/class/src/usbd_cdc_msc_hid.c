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
#include <assert.h>
#include "usbd_ioreq.h"
#include "usbd_cdc_msc_hid.h"
#define HID_DESC_OFFSET_SUBCLASS (6)
#define HID_DESC_OFFSET_PROTOCOL (7)
#define HID_DESC_OFFSET_SUBDESC (9)
#define HID_DESC_OFFSET_REPORT_DESC_LEN (16)
#define HID_DESC_OFFSET_MAX_PACKET_LO (22)
#define HID_DESC_OFFSET_MAX_PACKET_HI (23)
#define HID_DESC_OFFSET_POLLING_INTERVAL (24)
#define HID_DESC_OFFSET_MAX_PACKET_OUT_LO (29)
#define HID_DESC_OFFSET_MAX_PACKET_OUT_HI (30)
#define HID_DESC_OFFSET_POLLING_INTERVAL_OUT (31)
#define HID_SUBDESC_LEN (9)

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

//--------------------------------------------------------------------------
// Configuration Descriptor Template
typedef struct config_desc_header {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t wTotalLength_lo;
    uint8_t wTotalLength_hi;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} config_desc_header_t;

typedef struct {
    // Interface Association 
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bFirstInterface;
    uint8_t  bInterfaceCount;
    uint8_t  bFunctionClass;
    uint8_t  bFunctionSubClass;
    uint8_t  bFunctionProtocol;
    uint8_t  iFunction;
} interface_association_t;

typedef struct {
    // Interface Descriptor
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bInterfaceNumber;
    uint8_t  bAlternateSetting;
    uint8_t  bNumEndpoints;
    uint8_t  bInterfaceClass;
    uint8_t  bInterfaceSubClass;
    uint8_t  nInterfaceProtocol;
    uint8_t  iInterface;
} interface_descriptor_t;

typedef struct {
    // Functional descriptor
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  data[0];
} functional_descriptor_t;

typedef struct {
    // HID descriptor
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bcdHID_lo;
    uint8_t  bcdHID_hi;
    uint8_t  bCountryCode;
    uint8_t  bNumDescriptors;
    uint8_t  bReportDescriptorType;
    uint8_t  wDescriptorLength_lo;
    uint8_t  wDescriptorLength_hi;
} hid_descriptor_t;

typedef struct {
    // Endpoint descriptor
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint8_t  wMaxPacketSize_lo;
    uint8_t  wMaxPacketSize_hi;
    uint8_t  bInterval;
} endpoint_descriptor_t;


static config_desc_header_t * initial_config_desc_header(uint8_t *buffer) {
    config_desc_header_t *header = (config_desc_header_t *) buffer;
    header->bLength             = 0x09; // Configuration Descriptor size
    header->bDescriptorType     = USB_DESC_TYPE_CONFIGURATION; // Configuration
    header->wTotalLength_lo     = 0; // no of returned bytes
    header->wTotalLength_hi     = 0; // 
    header->bNumInterfaces      = 0; // number of interfaces
    header->bConfigurationValue = 0x01; // Configuration value
    header->iConfiguration      = 0x00; // Index of string descriptor describing the configuration
    header->bmAttributes        = 0x80; // bus powered; 0xc0 for self powered
    header->bMaxPower           = 0xfa; // in units of 2mA
    return header;
}

static size_t msc_descriptor(uint8_t *buffer, USBD_HandleTypeDef *pdev, USBD_MSC_BOT_HandleTypeDef *msc) {
    //==========================================================================
    // MSC only has 1 interface so doesn't need an IAD
    uint8_t *next = buffer;
    interface_descriptor_t *iface = (interface_descriptor_t *)next;
    iface->bLength = 0x09;                            // Interface Descriptor size
    iface->bDescriptorType = USB_DESC_TYPE_INTERFACE; // interface descriptor
    iface->bInterfaceNumber = msc->iface_num;         // Number of Interface
    iface->bAlternateSetting = 0x00;                  // Alternate setting          
    iface->bNumEndpoints = 0x02;                      //
    iface->bInterfaceClass = 0x08;                    // MSC Class
    iface->bInterfaceSubClass = 0x06;                 // SCSI transparent
    iface->nInterfaceProtocol = 0x50;                 //
    iface->iInterface = 0x00;                         //
    next += sizeof(interface_descriptor_t);

    uint32_t max_packet_size = usbd_msc_max_packet(pdev);
    
    endpoint_descriptor_t *in_ep_desc = (endpoint_descriptor_t *)next;
    in_ep_desc->bLength = 0x07;                                // Endpoint descriptor length
    in_ep_desc->bDescriptorType = USB_DESC_TYPE_ENDPOINT;      // Endpoint descriptor type
    in_ep_desc->bEndpointAddress = msc->in_ep;                      // endpoint number
    in_ep_desc->bmAttributes = 0x02;                           // Bulk endpoint type
    in_ep_desc->wMaxPacketSize_lo = LOBYTE(max_packet_size); //
    in_ep_desc->wMaxPacketSize_hi = HIBYTE(max_packet_size); //
    in_ep_desc->bInterval = 0x00;                              // ignore for Bulk transfer
    next += sizeof(endpoint_descriptor_t);

    endpoint_descriptor_t *out_ep_desc = (endpoint_descriptor_t *)next;
    out_ep_desc->bLength = 0x07;                                // Endpoint descriptor length
    out_ep_desc->bDescriptorType = USB_DESC_TYPE_ENDPOINT;      // Endpoint descriptor type
    out_ep_desc->bEndpointAddress = msc->out_ep;                     // endpoint number
    out_ep_desc->bmAttributes = 0x02;                           // Bulk endpoint type
    out_ep_desc->wMaxPacketSize_lo = LOBYTE(max_packet_size); //
    out_ep_desc->wMaxPacketSize_hi = HIBYTE(max_packet_size); //
    out_ep_desc->bInterval = 0x00;                              // ignore for Bulk transfer
    next += sizeof(endpoint_descriptor_t);

    return (uint32_t)next - (uint32_t)buffer;
};

static size_t cdc_descriptor(uint8_t *buffer, usbd_cdc_state_t *cdc) {
    //==========================================================================
    // Interface Association for CDC VCP
    uint8_t *next = buffer;
    interface_association_t *assoc = (interface_association_t *)next;
    assoc->bLength = 0x08;                              // 8 bytes  
    assoc->bDescriptorType = USB_DESC_TYPE_ASSOCIATION; // IAD  
    assoc->bFirstInterface = cdc->iface_num;            // first interface for this association  
    assoc->bInterfaceCount = 0x02;                      // nummber of interfaces for this association  
    assoc->bFunctionClass = 0x02;                       // Communication Interface Class  
    assoc->bFunctionSubClass = 0x02;                    // Abstract Control Model  
    assoc->bFunctionProtocol = 0x01;                    // Common AT commands  
    assoc->iFunction = 0x00;                            // index of string for this function      
    next += sizeof(interface_association_t);

    // Interface Descriptor
    interface_descriptor_t *iface = (interface_descriptor_t *)next;
    iface->bLength = 0x09;                            // Interface Descriptor size
    iface->bDescriptorType = USB_DESC_TYPE_INTERFACE; // interface descriptor
    iface->bInterfaceNumber = cdc->iface_num;         // Number of Interface
    iface->bAlternateSetting = 0x00;                  // Alternate setting          
    iface->bNumEndpoints = 0x01;                      //
    iface->bInterfaceClass = 0x02;                    // Communication Interface Class
    iface->bInterfaceSubClass = 0x02;                 // Abstract Control Model
    iface->nInterfaceProtocol = 0x01;                 // Common AT commands
    iface->iInterface = 0x00;                         //
    next += sizeof(interface_descriptor_t);

    // Header Functional Descriptor
    functional_descriptor_t *header = (functional_descriptor_t *)next;
    header->bLength = 0x05;             // bLength: Endpoint Descriptor size
    header->bDescriptorType = 0x24;     // bDescriptorType: CS_INTERFACE
    header->bDescriptorSubtype = 0x00;  // bDescriptorSubtype: Header Func Desc
    header->data[0] = 0x10;             // bcdCDC: spec release number
    header->data[1] = 0x01;             // ?
    next += header->bLength;

    // Call Management Functional Descriptor
    functional_descriptor_t *call_management = (functional_descriptor_t *)next;
    call_management->bLength = 0x05;             // bLength: Endpoint Descriptor size
    call_management->bDescriptorType = 0x24;     // bDescriptorType: CS_INTERFACE
    call_management->bDescriptorSubtype = 0x01;  // bDescriptorSubtype: Call Management Func Desc
    call_management->data[0] = 0x00;             // bmCapabilities: D0+D1
    call_management->data[1] = cdc->iface_num+1; // bDataInterface: 1
    next += call_management->bLength;

    // ACM Functional Descriptor
    functional_descriptor_t *acm_functional = (functional_descriptor_t *)next;
    acm_functional->bLength = 0x04;            // bLength: Endpoint Descriptor size
    acm_functional->bDescriptorType = 0x24;    // bDescriptorType: CS_INTERFACE
    acm_functional->bDescriptorSubtype = 0x02; // bDescriptorSubtype: Abstract Control Management desc
    acm_functional->data[0] = 0x02;            // bmCapabilities
    next += acm_functional->bLength;

    // Union Functional Descriptor
    functional_descriptor_t *union_functional = (functional_descriptor_t *)next;
    union_functional->bLength = 0x05;             // bLength: Endpoint Descriptor size
    union_functional->bDescriptorType = 0x24;     // bDescriptorType: CS_INTERFACE
    union_functional->bDescriptorSubtype = 0x06;  // bDescriptorSubtype: Union func desc
    union_functional->data[0] = cdc->iface_num;   // bMasterInterface: Communication class interface
    union_functional->data[1] = cdc->iface_num+1; // bSlaveInterface0: Data Class Interface
    next += union_functional->bLength;

    uint8_t interval; // polling interval in frames of 1ms
    if (cdc->usbd->pdev->dev_speed == USBD_SPEED_HIGH) {
        interval = 0x09;
    } else {
        interval = 0x20;
    }

    // Command Endpoint Descriptor
    endpoint_descriptor_t *cmd_ep_desc = (endpoint_descriptor_t *)next;
    cmd_ep_desc->bLength = 0x07;                                // Endpoint descriptor length
    cmd_ep_desc->bDescriptorType = USB_DESC_TYPE_ENDPOINT;      // Endpoint descriptor type
    cmd_ep_desc->bEndpointAddress = cdc->cmd_ep;                     // endpoint number
    cmd_ep_desc->bmAttributes = 0x03;                           // Interrupt
    cmd_ep_desc->wMaxPacketSize_lo = LOBYTE(CDC_CMD_PACKET_SIZE); //
    cmd_ep_desc->wMaxPacketSize_hi = HIBYTE(CDC_CMD_PACKET_SIZE); //
    cmd_ep_desc->bInterval = interval;                          // polling interval in frames of 1ms
    next += sizeof(endpoint_descriptor_t);

    // Data class interface descriptor
    interface_descriptor_t *data_interface = (interface_descriptor_t *)next;
    data_interface->bLength = 0x09;                            // Interface Descriptor size
    data_interface->bDescriptorType = USB_DESC_TYPE_INTERFACE; // interface descriptor
    data_interface->bInterfaceNumber = cdc->iface_num+1;       // Number of Interface
    data_interface->bAlternateSetting = 0x00;                  // Alternate setting          
    data_interface->bNumEndpoints = 0x02;                      //
    data_interface->bInterfaceClass = 0x0A;                    // CDC
    data_interface->bInterfaceSubClass = 0x00;                 //
    data_interface->nInterfaceProtocol = 0x00;                 //
    data_interface->iInterface = 0x00;                         //
    next += sizeof(interface_descriptor_t);

    uint32_t max_packet_size = usbd_cdc_max_packet(cdc->usbd->pdev);
    
    // Endpoint OUT Descriptor
    endpoint_descriptor_t *out_ep_desc = (endpoint_descriptor_t *)next;
    out_ep_desc->bLength = 0x07;                                // Endpoint descriptor length
    out_ep_desc->bDescriptorType = USB_DESC_TYPE_ENDPOINT;      // Endpoint descriptor type
    out_ep_desc->bEndpointAddress = cdc->out_ep;                // endpoint number
    out_ep_desc->bmAttributes = 0x02;                           // Bulk endpoint type
    out_ep_desc->wMaxPacketSize_lo = LOBYTE(max_packet_size); //
    out_ep_desc->wMaxPacketSize_hi = HIBYTE(max_packet_size); //
    out_ep_desc->bInterval = 0x00;                              // ignore for Bulk transfer
    next += sizeof(endpoint_descriptor_t);

    // Endpoint IN Descriptor
    endpoint_descriptor_t *in_ep_desc = (endpoint_descriptor_t *)next;
    in_ep_desc->bLength = 0x07;                                // Endpoint descriptor length
    in_ep_desc->bDescriptorType = USB_DESC_TYPE_ENDPOINT;      // Endpoint descriptor type
    in_ep_desc->bEndpointAddress = cdc->in_ep;                      // endpoint number
    in_ep_desc->bmAttributes = 0x02;                           // Bulk endpoint type
    in_ep_desc->wMaxPacketSize_lo = LOBYTE(max_packet_size); //
    in_ep_desc->wMaxPacketSize_hi = HIBYTE(max_packet_size); //
    in_ep_desc->bInterval = 0x00;                              // ignore for Bulk transfer
    next += sizeof(endpoint_descriptor_t);

    return (uint32_t)next - (uint32_t)buffer;
};

static size_t hid_descriptor(uint8_t *buffer, usbd_hid_state_t *hid) {
    //==========================================================================
    // HID only has 1 interface so doesn't need an IAD
    uint8_t *next = buffer;
    interface_descriptor_t *iface = (interface_descriptor_t *)next;
    iface->bLength = 0x09;                            // Interface Descriptor size
    iface->bDescriptorType = USB_DESC_TYPE_INTERFACE; // interface descriptor
    iface->bInterfaceNumber = hid->iface_num;         // Number of Interface
    iface->bAlternateSetting = 0x00;                  // Alternate setting          
    iface->bNumEndpoints = 0x02;                      //
    iface->bInterfaceClass = 0x03;                    // HID Class
    iface->bInterfaceSubClass = 0x01;                 // 0=no sub class, 1=boot
    iface->nInterfaceProtocol = 0x02;                 // 0=none, 1=keyboard, 2=mouse
    iface->iInterface = 0x00;                         //
    next += sizeof(interface_descriptor_t);

    hid_descriptor_t *hid_desc = (hid_descriptor_t *)next;
    hid_desc->bLength = 0x09;                             // HID Descriptor size
    hid_desc->bDescriptorType = HID_DESCRIPTOR_TYPE;      // HID
    hid_desc->bcdHID_lo = 0x11;                           // HID Class Spec release number
    hid_desc->bcdHID_hi = 0x01;                           // 
    hid_desc->bCountryCode = 0x00;                        // Hardware target country
    hid_desc->bNumDescriptors = 0x01;                     // Number of HID class descriptors to follow
    hid_desc->bReportDescriptorType = 0x22;               // 
    hid_desc->wDescriptorLength_lo = USBD_HID_MOUSE_REPORT_DESC_SIZE; // Length of Report descriptor
    hid_desc->wDescriptorLength_hi = 0x00;
    next += sizeof(hid_descriptor_t);

    endpoint_descriptor_t *in_ep_desc = (endpoint_descriptor_t *)next;
    in_ep_desc->bLength = 0x07;                                // Endpoint descriptor length
    in_ep_desc->bDescriptorType = USB_DESC_TYPE_ENDPOINT;      // Endpoint descriptor type
    in_ep_desc->bEndpointAddress = hid->in_ep;                 // endpoint number
    in_ep_desc->bmAttributes = 0x03;                           // Interrupt endpoint type
    in_ep_desc->wMaxPacketSize_lo = LOBYTE(USBD_HID_MOUSE_MAX_PACKET); //
    in_ep_desc->wMaxPacketSize_hi = HIBYTE(USBD_HID_MOUSE_MAX_PACKET); //
    in_ep_desc->bInterval = 0x08;                              // Polling interval
    next += sizeof(endpoint_descriptor_t);

    endpoint_descriptor_t *out_ep_desc = (endpoint_descriptor_t *)next;
    out_ep_desc->bLength = 0x07;                                // Endpoint descriptor length
    out_ep_desc->bDescriptorType = USB_DESC_TYPE_ENDPOINT;      // Endpoint descriptor type
    out_ep_desc->bEndpointAddress = hid->out_ep;                // endpoint number
    out_ep_desc->bmAttributes = 0x03;                           // Interrupt endpoint type
    out_ep_desc->wMaxPacketSize_lo = LOBYTE(USBD_HID_MOUSE_MAX_PACKET); //
    out_ep_desc->wMaxPacketSize_hi = HIBYTE(USBD_HID_MOUSE_MAX_PACKET); //
    out_ep_desc->bInterval = 0x08;                              // Polling interval
    next += sizeof(endpoint_descriptor_t);

    return (uint32_t)next - (uint32_t)buffer;
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

// return the currest usb mode details
uint8_t USBD_CDC_Enabled(usbd_cdc_msc_hid_state_t *usbd) {
    uint8_t count = 0;
    for (uint8_t i = 0; i < MICROPY_HW_USB_NUM_CDC; i++) {
        if (usbd->cdc[i]->iface_num != -1) {
            count += 1;
        }
    }
    return count;
}
bool USBD_HID_Enabled(usbd_cdc_msc_hid_state_t *usbd) {
    return usbd->hid->iface_num != -1;
}
bool USBD_MSC_Enabled(usbd_cdc_msc_hid_state_t *usbd) {
    return usbd->MSC_BOT_ClassData.iface_num != -1;
}

int USBD_SelectMode(usbd_cdc_msc_hid_state_t *usbd, uint8_t cdc, bool msc, bool hid, USBD_HID_ModeInfoTypeDef *hid_info) {
    // save mode
    memset(usbd->usbd_config_desc, 0, MAX_TEMPLATE_CONFIG_DESC_SIZE);

    config_desc_header_t *header = initial_config_desc_header(usbd->usbd_config_desc);
    
    uint8_t *descriptors = ((uint8_t *)header) + sizeof(config_desc_header_t);
    uint8_t next_ep_out = 0x01;
    uint8_t next_ep_in = 0x81;

    if (cdc > MICROPY_HW_USB_NUM_CDC) {
        return -1; // Settings not supported
    }
    
    for (uint8_t i = 0; i < MICROPY_HW_USB_NUM_CDC; i++) {
        if (i < cdc) {
            // Enable CDC Interface
            usbd->cdc[i]->iface_num = header->bNumInterfaces;
            header->bNumInterfaces += 2;

            usbd->cdc[i]->in_ep = next_ep_in++;
            usbd->cdc[i]->out_ep = next_ep_out++;
            usbd->cdc[i]->cmd_ep = next_ep_in++;

            descriptors += cdc_descriptor(descriptors, usbd->cdc[i]);
        } else {
            // Disabled
            usbd->cdc[i]->iface_num = -1;
        }
    }

    if (msc) {
        usbd->MSC_BOT_ClassData.iface_num = header->bNumInterfaces++;
        usbd->MSC_BOT_ClassData.in_ep = next_ep_in++;;
        usbd->MSC_BOT_ClassData.out_ep = next_ep_out++;;
        descriptors += msc_descriptor(descriptors, usbd->pdev, &usbd->MSC_BOT_ClassData);
    } else {
        usbd->MSC_BOT_ClassData.iface_num = -1;
    }

    if (hid) {
        usbd->hid->iface_num = header->bNumInterfaces++;
        usbd->hid->in_ep = next_ep_in++;;
        usbd->hid->out_ep = next_ep_out++;;
        descriptors += hid_descriptor(descriptors, usbd->hid);
        
        // configure the HID descriptor
        uint8_t *hid_desc = usbd->hid->desc;
        hid_desc[HID_DESC_OFFSET_SUBCLASS] = hid_info->subclass;
        hid_desc[HID_DESC_OFFSET_PROTOCOL] = hid_info->protocol;
        hid_desc[HID_DESC_OFFSET_REPORT_DESC_LEN] = hid_info->report_desc_len;
        hid_desc[HID_DESC_OFFSET_MAX_PACKET_LO] = hid_info->max_packet_len;
        hid_desc[HID_DESC_OFFSET_MAX_PACKET_HI] = 0;
        hid_desc[HID_DESC_OFFSET_POLLING_INTERVAL] = hid_info->polling_interval;
        hid_desc[HID_DESC_OFFSET_MAX_PACKET_OUT_LO] = hid_info->max_packet_len;
        hid_desc[HID_DESC_OFFSET_MAX_PACKET_OUT_HI] = 0;
        hid_desc[HID_DESC_OFFSET_POLLING_INTERVAL_OUT] = hid_info->polling_interval;
        usbd->hid->report_desc = hid_info->report_desc;

    } else {
        usbd->hid->iface_num = -1;
    }

    usbd->usbd_config_desc_size = (uint32_t)descriptors - (uint32_t)usbd->usbd_config_desc;
    
    header->wTotalLength_lo = LOBYTE(usbd->usbd_config_desc_size);
    header->wTotalLength_hi = HIBYTE(usbd->usbd_config_desc_size);
    
    usbd->used_endpoints = (next_ep_out > (next_ep_in & 0x7F))? next_ep_out : (next_ep_in & 0x7F);
    
    // Check for buffer overflow
    assert(usbd->usbd_config_desc_size <= MAX_TEMPLATE_CONFIG_DESC_SIZE);
    
    return 0;
}

static uint8_t USBD_CDC_MSC_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    #if !USBD_SUPPORT_HS_MODE
    if (pdev->dev_speed == USBD_SPEED_HIGH) {
        // can't handle high speed
        return 1;
    }
    #endif

    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;

    for (uint8_t i = 0; i < MICROPY_HW_USB_NUM_CDC; i++) {
        if (usbd->cdc[i]->iface_num != -1) {
        // CDC VCP components
        int mp = usbd_cdc_max_packet(pdev);

        // Open EP IN
        USBD_LL_OpenEP(pdev,
                       usbd->cdc[i]->in_ep,
                       USBD_EP_TYPE_BULK,
                       mp);

        // Open EP OUT
        USBD_LL_OpenEP(pdev,
                       usbd->cdc[i]->out_ep,
                       USBD_EP_TYPE_BULK,
                       mp);

        // Open Command IN EP
        USBD_LL_OpenEP(pdev,
                       usbd->cdc[i]->cmd_ep,
                       USBD_EP_TYPE_INTR,
                       CDC_CMD_PACKET_SIZE);

        // Init state
        usbd->cdc[i]->usbd = usbd;
        usbd->cdc[i]->cur_request = 0xff;
        usbd->cdc[i]->tx_in_progress = 0;

        // Init physical Interface components
        uint8_t *buf = usbd_cdc_init(usbd->cdc[i]);

        if (i == 0) {
            // First CDC is attached to repl by default
            usbd_cdc_attach_to_repl(usbd->cdc[i], true);
        }

        // Prepare Out endpoint to receive next packet
        USBD_LL_PrepareReceive(pdev, usbd->cdc[i]->out_ep, buf, mp);

        }
    }

    if (usbd->MSC_BOT_ClassData.iface_num != -1) {
        // MSC component

        int mp = usbd_msc_max_packet(pdev);

        // Open EP OUT
        USBD_LL_OpenEP(pdev,
                       usbd->MSC_BOT_ClassData.out_ep,
                       USBD_EP_TYPE_BULK,
                       mp);

        // Open EP IN
        USBD_LL_OpenEP(pdev,
                       usbd->MSC_BOT_ClassData.in_ep,
                       USBD_EP_TYPE_BULK,
                       mp);

        // Init the BOT layer
        MSC_BOT_Init(pdev);
    }

    if (usbd->hid->iface_num != -1) {
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

    return 0;
}

static uint8_t USBD_CDC_MSC_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;

    for (uint8_t i = 0; i < MICROPY_HW_USB_NUM_CDC; i++) {
        if (usbd->cdc[i]->iface_num != -1) {
            // CDC VCP component

            // close endpoints
            USBD_LL_CloseEP(pdev, usbd->cdc[i]->in_ep);
            USBD_LL_CloseEP(pdev, usbd->cdc[i]->out_ep);
            USBD_LL_CloseEP(pdev, usbd->cdc[i]->cmd_ep);
        }
    }

    if (usbd->MSC_BOT_ClassData.iface_num != -1) {
        // MSC component

        // close endpoints
        USBD_LL_CloseEP(pdev, usbd->MSC_BOT_ClassData.in_ep);
        USBD_LL_CloseEP(pdev, usbd->MSC_BOT_ClassData.out_ep);

        // DeInit the BOT layer
        MSC_BOT_DeInit(pdev);
    }

    if (usbd->hid->iface_num != -1) {
        // HID component

        // close endpoints
        USBD_LL_CloseEP(pdev, usbd->hid->in_ep);
        USBD_LL_CloseEP(pdev, usbd->hid->out_ep);
    }

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
    int8_t recipient_cdc = -1;
    int8_t recipient_hid = -1;
    int8_t recipient_msc = -1;

    switch (req->bmRequest & USB_REQ_RECIPIENT_MASK) {
        case USB_REQ_RECIPIENT_INTERFACE: {
            uint16_t iface = req->wIndex;

            for (uint8_t i = 0; i < MICROPY_HW_USB_NUM_CDC; i++) {
                if (iface == usbd->cdc[i]->iface_num) {
                    recipient_cdc = i;
                    break;
                }
            }
            if (iface == usbd->hid->iface_num) {
                recipient_hid = 1;
            }
            else if (iface == usbd->MSC_BOT_ClassData.iface_num) {
                recipient_msc = 1;
            }
            break;
        }

        case USB_REQ_RECIPIENT_ENDPOINT: {
            uint8_t ep = req->wIndex & 0x7f;
            for (uint8_t i = 0; i < MICROPY_HW_USB_NUM_CDC; i++) {
                if ((ep == usbd->cdc[i]->out_ep) || (ep == (usbd->cdc[i]->cmd_ep & 0x7f))) {
                    recipient_cdc = i;
                    break;
                }
            }
            if (ep == usbd->MSC_BOT_ClassData.out_ep) {
                recipient_msc = 1;
            } else if (ep == usbd->hid->out_ep) {
                recipient_hid = 1;
            }
            break;
        }
    }

    // Fail the request if we didn't have a valid recipient
    if ((recipient_cdc == -1) && (recipient_msc == -1) &&  (recipient_hid == -1)) {
        USBD_CtlError(pdev, req);
        return USBD_FAIL;
    }

    switch (req->bmRequest & USB_REQ_TYPE_MASK) {

        // Class request
        case USB_REQ_TYPE_CLASS:
            if (recipient_cdc != -1) {
                if (req->wLength) {
                    if (req->bmRequest & 0x80) {
                        // device-to-host request
                        usbd_cdc_control(usbd->cdc[recipient_cdc], req->bRequest, (uint8_t*)usbd->cdc[recipient_cdc]->ctl_packet_buf, req->wLength);
                        USBD_CtlSendData(pdev, (uint8_t*)usbd->cdc[recipient_cdc]->ctl_packet_buf, req->wLength);
                    } else {
                        // host-to-device request
                        usbd->cdc[recipient_cdc]->cur_request = req->bRequest;
                        usbd->cdc[recipient_cdc]->cur_length = req->wLength;
                        USBD_CtlPrepareRx(pdev, (uint8_t*)usbd->cdc[recipient_cdc]->ctl_packet_buf, req->wLength);
                    }
                } else {
                    // Not a Data request
                    // Transfer the command to the interface layer
                    return usbd_cdc_control(usbd->cdc[recipient_cdc], req->bRequest, NULL, req->wValue);
                }
            } else if (recipient_msc != -1) {
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
            } else if (recipient_hid != -1) {
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
            break;

        case USB_REQ_TYPE_STANDARD:
            if (recipient_msc != -1) {
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
                            USBD_LL_OpenEP(pdev, usbd->MSC_BOT_ClassData.in_ep, USBD_EP_TYPE_BULK, usbd_msc_max_packet(pdev));
                        } else {
                            // Open EP OUT
                            USBD_LL_OpenEP(pdev, usbd->MSC_BOT_ClassData.out_ep, USBD_EP_TYPE_BULK, usbd_msc_max_packet(pdev));
                        }
                        // Handle BOT error
                        MSC_BOT_CplClrFeature(pdev, (uint8_t)req->wIndex);
                        break;
                }
            } else if (recipient_hid != -1) {
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
    for (uint8_t i = 0; i < MICROPY_HW_USB_NUM_CDC; i++) {
        if (usbd->cdc[i] != NULL && usbd->cdc[i]->cur_request != 0xff) {
            usbd_cdc_control(usbd->cdc[i], usbd->cdc[i]->cur_request, (uint8_t*)usbd->cdc[i]->ctl_packet_buf, usbd->cdc[i]->cur_length);
            usbd->cdc[i]->cur_request = 0xff;
        }
    }
    return USBD_OK;
}

static uint8_t USBD_CDC_MSC_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;
    for (uint8_t i = 0; i < MICROPY_HW_USB_NUM_CDC; i++) {
        if ((epnum == (usbd->cdc[i]->in_ep & 0x7f)) || (epnum == (usbd->cdc[i]->cmd_ep & 0x7f))) {
            usbd->cdc[i]->tx_in_progress = 0;
            return USBD_OK;
        }
    }

    if (epnum == (usbd->MSC_BOT_ClassData.in_ep & 0x7f)) {
        MSC_BOT_DataIn(pdev, epnum);
        return USBD_OK;
    } 
    
    if (epnum == (usbd->hid->in_ep & 0x7f)) {
        /* Ensure that the FIFO is empty before a new transfer, this condition could
        be caused by  a new transfer before the end of the previous transfer */
        usbd->hid->state = HID_IDLE;
        return USBD_OK;
    }

    return USBD_OK;
}

static uint8_t USBD_CDC_MSC_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;

    for (uint8_t i = 0; i < MICROPY_HW_USB_NUM_CDC; i++) {
        if (epnum == (usbd->cdc[i]->out_ep & 0x7f)){
            /* Get the received data length */
            size_t len = USBD_LL_GetRxDataSize (pdev, epnum);

            /* USB data will be immediately processed, this allow next USB traffic being
            NAKed till the end of the application Xfer */
            usbd_cdc_receive(usbd->cdc[i], len);

            return USBD_OK;
        }
    }

    if (epnum == (usbd->MSC_BOT_ClassData.out_ep & 0x7f)) {
        MSC_BOT_DataOut(pdev, epnum);
        return USBD_OK;
    } 
    
    if (epnum == (usbd->hid->out_ep & 0x7f)) {
        size_t len = USBD_LL_GetRxDataSize(pdev, epnum);
        usbd_hid_receive(usbd->hid, len);
    }

    return USBD_OK;
}

static uint8_t *USBD_CDC_MSC_HID_GetCfgDesc(USBD_HandleTypeDef *pdev, uint16_t *length) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;
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

uint8_t USBD_HID_SetNAK(usbd_hid_state_t *hid) {
    // get USBx object from pdev (needed for USBx_OUTEP macro below)
    PCD_HandleTypeDef *hpcd = hid->usbd->pdev->pData;
    USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
    // set NAK on HID OUT endpoint
    USBx_OUTEP(hid->usbd->hid->out_ep)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
    return USBD_OK;
}

uint8_t USBD_HID_ClearNAK(usbd_hid_state_t *hid) {
    // get USBx object from pdev (needed for USBx_OUTEP macro below)
    PCD_HandleTypeDef *hpcd = hid->usbd->pdev->pData;
    USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
    // clear NAK on HID OUT endpoint
    USBx_OUTEP(hid->usbd->hid->out_ep)->DOEPCTL |= USB_OTG_DOEPCTL_CNAK;
    return USBD_OK;
}

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
