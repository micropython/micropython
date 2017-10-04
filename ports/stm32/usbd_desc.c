/*
 * This file is part of the MicroPython project, http://micropython.org/
 */

/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Src/usbd_desc.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    26-February-2014
  * @brief   This file provides the USBD descriptors and string formating method.
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

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_conf.h"

// need these headers just for MP_HAL_UNIQUE_ID_ADDRESS
#include "py/misc.h"
#include "py/mphal.h"

// So we don't clash with existing ST boards, we use the unofficial FOSS VID.
// This needs a proper solution.
#define USBD_VID                      0xf055
#define USBD_PID                      0x9800
#define USBD_LANGID_STRING            0x409
#define USBD_MANUFACTURER_STRING      "MicroPython"
#define USBD_PRODUCT_HS_STRING        "Pyboard Virtual Comm Port in HS Mode"
#define USBD_PRODUCT_FS_STRING        "Pyboard Virtual Comm Port in FS Mode"
#define USBD_CONFIGURATION_HS_STRING  "Pyboard Config"
#define USBD_INTERFACE_HS_STRING      "Pyboard Interface"
#define USBD_CONFIGURATION_FS_STRING  "Pyboard Config"
#define USBD_INTERFACE_FS_STRING      "Pyboard Interface"

__ALIGN_BEGIN static const uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
    USB_LEN_LANGID_STR_DESC,
    USB_DESC_TYPE_STRING,
    LOBYTE(USBD_LANGID_STRING),
    HIBYTE(USBD_LANGID_STRING),
};

// set the VID, PID and device release number
void USBD_SetVIDPIDRelease(usbd_cdc_msc_hid_state_t *usbd, uint16_t vid, uint16_t pid, uint16_t device_release_num, int cdc_only) {
    uint8_t *dev_desc = &usbd->usbd_device_desc[0];

    dev_desc[0] = USB_LEN_DEV_DESC; // bLength
    dev_desc[1] = USB_DESC_TYPE_DEVICE; // bDescriptorType
    dev_desc[2] = 0x00; // bcdUSB
    dev_desc[3] = 0x02; // bcdUSB
    if (cdc_only) {
        // Make it look like a Communications device if we're only
        // using CDC. Otherwise, windows gets confused when we tell it that
        // its a composite device with only a cdc serial interface.
        dev_desc[4] = 0x02; // bDeviceClass
        dev_desc[5] = 0x00; // bDeviceSubClass
        dev_desc[6] = 0x00; // bDeviceProtocol
    } else {
        // For the other modes, we make this look like a composite device.
        dev_desc[4] = 0xef; // bDeviceClass: Miscellaneous Device Class
        dev_desc[5] = 0x02; // bDeviceSubClass: Common Class
        dev_desc[6] = 0x01; // bDeviceProtocol: Interface Association Descriptor
    }
    dev_desc[7] = USB_MAX_EP0_SIZE; // bMaxPacketSize
    dev_desc[8] = LOBYTE(vid); // idVendor
    dev_desc[9] = HIBYTE(vid); // idVendor
    dev_desc[10] = LOBYTE(pid); // idVendor
    dev_desc[11] = HIBYTE(pid); // idVendor
    dev_desc[12] = LOBYTE(device_release_num); // bcdDevice
    dev_desc[13] = HIBYTE(device_release_num); // bcdDevice
    dev_desc[14] = USBD_IDX_MFC_STR; // Index of manufacturer string
    dev_desc[15] = USBD_IDX_PRODUCT_STR; // Index of product string
    dev_desc[16] = USBD_IDX_SERIAL_STR; // Index of serial number string
    dev_desc[17] = USBD_MAX_NUM_CONFIGURATION; // bNumConfigurations
}

/**
  * @brief  Returns the device descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
STATIC uint8_t *USBD_DeviceDescriptor(USBD_HandleTypeDef *pdev, uint16_t *length) {
    uint8_t *dev_desc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->usbd_device_desc;
    *length = USB_LEN_DEV_DESC;
    return dev_desc;
}

/**
  * @brief  Returns the LangID string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
STATIC uint8_t *USBD_LangIDStrDescriptor(USBD_HandleTypeDef *pdev, uint16_t *length) {
    *length = sizeof(USBD_LangIDDesc);
    return (uint8_t*)USBD_LangIDDesc; // the data should only be read from this buf
}

/**
  * @brief  Returns the product string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
STATIC uint8_t *USBD_ProductStrDescriptor(USBD_HandleTypeDef *pdev, uint16_t *length) {
    uint8_t *str_desc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->usbd_str_desc;
    if (pdev->dev_speed == USBD_SPEED_HIGH) {
        USBD_GetString((uint8_t *)USBD_PRODUCT_HS_STRING, str_desc, length);
    } else {
        USBD_GetString((uint8_t *)USBD_PRODUCT_FS_STRING, str_desc, length);
    }
    return str_desc;
}

/**
  * @brief  Returns the manufacturer string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
STATIC uint8_t *USBD_ManufacturerStrDescriptor(USBD_HandleTypeDef *pdev, uint16_t *length) {
    uint8_t *str_desc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->usbd_str_desc;
    USBD_GetString((uint8_t *)USBD_MANUFACTURER_STRING, str_desc, length);
    return str_desc;
}

/**
  * @brief  Returns the serial number string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
STATIC uint8_t *USBD_SerialStrDescriptor(USBD_HandleTypeDef *pdev, uint16_t *length) {
    // This document: http://www.usb.org/developers/docs/devclass_docs/usbmassbulk_10.pdf
    // says that the serial number has to be at least 12 digits long and that
    // the last 12 digits need to be unique. It also stipulates that the valid
    // character set is that of upper-case hexadecimal digits.
    //
    // The onboard DFU bootloader produces a 12-digit serial number based on
    // the 96-bit unique ID, so for consistency we go with this algorithm.
    // You can see the serial number if you do:
    //
    //     dfu-util -l
    //
    // See: https://my.st.com/52d187b7 for the algorithim used.

    uint8_t *id = (uint8_t *)MP_HAL_UNIQUE_ID_ADDRESS;
    char serial_buf[16];
    snprintf(serial_buf, sizeof(serial_buf),
        "%02X%02X%02X%02X%02X%02X",
        id[11], id[10] + id[2], id[9], id[8] + id[0], id[7], id[6]);

    uint8_t *str_desc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->usbd_str_desc;
    USBD_GetString((uint8_t *)serial_buf, str_desc, length);
    return str_desc;
}

/**
  * @brief  Returns the configuration string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
STATIC uint8_t *USBD_ConfigStrDescriptor(USBD_HandleTypeDef *pdev, uint16_t *length) {
    uint8_t *str_desc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->usbd_str_desc;
    if (pdev->dev_speed == USBD_SPEED_HIGH) {
        USBD_GetString((uint8_t *)USBD_CONFIGURATION_HS_STRING, str_desc, length);
    } else {
        USBD_GetString((uint8_t *)USBD_CONFIGURATION_FS_STRING, str_desc, length);
    }
    return str_desc;
}

/**
  * @brief  Returns the interface string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
STATIC uint8_t *USBD_InterfaceStrDescriptor(USBD_HandleTypeDef *pdev, uint16_t *length) {
    uint8_t *str_desc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->usbd_str_desc;
    if (pdev->dev_speed == USBD_SPEED_HIGH) {
        USBD_GetString((uint8_t *)USBD_INTERFACE_HS_STRING, str_desc, length);
    } else {
        USBD_GetString((uint8_t *)USBD_INTERFACE_FS_STRING, str_desc, length);
    }
    return str_desc;
}

const USBD_DescriptorsTypeDef USBD_Descriptors = {
    USBD_DeviceDescriptor,
    USBD_LangIDStrDescriptor,
    USBD_ManufacturerStrDescriptor,
    USBD_ProductStrDescriptor,
    USBD_SerialStrDescriptor,
    USBD_ConfigStrDescriptor,
    USBD_InterfaceStrDescriptor,
};

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
