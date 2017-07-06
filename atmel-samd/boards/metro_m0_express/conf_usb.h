
#include <stdbool.h>
#include <stdint.h>

#include "asf/common/services/usb/class/cdc/usb_protocol_cdc.h"

#ifndef CONF_USB_H_INCLUDED
#define CONF_USB_H_INCLUDED

#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                  \
	(USB_CONFIG_ATTR_BUS_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
#ifndef USB_DEVICE_MANUFACTURE_NAME
#  define USB_DEVICE_MANUFACTURE_NAME "Adafruit Industries"
#endif

#ifndef USB_DEVICE_PRODUCT_NAME
#  define USB_DEVICE_PRODUCT_NAME "Metro M0 Express"
#endif
// #define  USB_DEVICE_SERIAL_NAME           "12...EF"
#define  USB_DEVICE_GET_SERIAL_NAME_POINTER  serial_number
#define  USB_DEVICE_GET_SERIAL_NAME_LENGTH   32
extern char serial_number[USB_DEVICE_GET_SERIAL_NAME_LENGTH];

//! Control endpoint size
#define  USB_DEVICE_EP_CTRL_SIZE       64

//! Interfaces for this device (CDC COM + CDC DATA + MSC + HID mouse + HID kbd)
#define  USB_DEVICE_NB_INTERFACE       5

// (3 | USB_EP_DIR_IN)  // CDC Notify endpoint
// (4 | USB_EP_DIR_IN)  // CDC TX
// (5 | USB_EP_DIR_OUT) // CDC RX
// (1 | USB_EP_DIR_IN)  // MSC IN
// (2 | USB_EP_DIR_OUT) // MSC OUT
// (6 | USB_EP_DIR_IN)  // HID mouse report
// (7 | USB_EP_DIR_IN)  // HID keyboard report
#define  USB_DEVICE_MAX_EP           7

#define UDI_CDC_PORT_NB 1
#define UDI_CDC_ENABLE_EXT(port) mp_cdc_enable(port)
extern bool mp_cdc_enable(uint8_t port);
#define UDI_CDC_DISABLE_EXT(port) mp_cdc_disable(port)
extern void mp_cdc_disable(uint8_t port);
#define  UDI_CDC_LOW_RATE

#define  UDI_CDC_DEFAULT_RATE             115200
#define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
#define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
#define  UDI_CDC_DEFAULT_DATABITS         8

#define  UDI_CDC_RX_NOTIFY(port)           usb_rx_notify()
void usb_rx_notify(void);
#define  UDI_CDC_SET_CODING_EXT(port,cfg) usb_coding_notify(port, cfg)
void usb_coding_notify(uint8_t port, usb_cdc_line_coding_t* coding);
#define  UDI_CDC_SET_DTR_EXT(port,set) usb_dtr_notify(port, set)
void usb_dtr_notify(uint8_t port, bool set);
#define  UDI_CDC_SET_RTS_EXT(port,set) usb_rts_notify(port, set)
void usb_rts_notify(uint8_t port, bool set);

/**
 * USB CDC low level configuration
 * In standalone these configurations are defined by the CDC module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition

#define  UDI_CDC_COMM_EP_0             (3 | USB_EP_DIR_IN)  // Notify endpoint
#define  UDI_CDC_DATA_EP_IN_0          (4 | USB_EP_DIR_IN)  // TX
#define  UDI_CDC_DATA_EP_OUT_0         (5 | USB_EP_DIR_OUT) // RX

//! Interface numbers
#define  UDI_CDC_COMM_IFACE_NUMBER_0   0
#define  UDI_CDC_DATA_IFACE_NUMBER_0   1

/**
 * Configuration of MSC interface
 * @{
 */
//! Vendor name and Product version of MSC interface
#define UDI_MSC_GLOBAL_VENDOR_ID            \
   'A', 'T', 'M', 'E', 'L', ' ', ' ', ' '
#define UDI_MSC_GLOBAL_PRODUCT_VERSION            \
   '1', '.', '0', '0'

//! Interface callback definition
#define  UDI_MSC_ENABLE_EXT()          mp_msc_enable()
extern bool mp_msc_enable(void);
#define  UDI_MSC_DISABLE_EXT()         mp_msc_disable()
extern void mp_msc_disable(void);

//! Enable id string of interface to add an extra USB string
#define  UDI_MSC_STRING_ID                5

/**
 * USB MSC low level configuration
 * In standalone these configurations are defined by the MSC module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_MSC_EP_IN                 (1 | USB_EP_DIR_IN)
#define  UDI_MSC_EP_OUT                (2 | USB_EP_DIR_OUT)

//! Interface number
#define  UDI_MSC_IFACE_NUMBER          2

/**
 * Configuration of HID Mouse interface
 * @{
 */
//! Interface callback definition
#define  UDI_HID_MOUSE_ENABLE_EXT()       mp_mouse_enable()
extern bool mp_mouse_enable(void);
#define  UDI_HID_MOUSE_DISABLE_EXT()      mp_mouse_disable()
extern void mp_mouse_disable(void);

//! Enable id string of interface to add an extra USB string
#define  UDI_HID_MOUSE_STRING_ID          6

/**
 * USB HID Mouse low level configuration
 * In standalone these configurations are defined by the HID Mouse module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_HID_MOUSE_EP_IN           (6 | USB_EP_DIR_IN)

//! Interface number
#define  UDI_HID_MOUSE_IFACE_NUMBER    3
//@}
//@}

/**
 * Configuration of HID Keyboard interface
 * @{
 */
//! Interface callback definition
#define  UDI_HID_KBD_ENABLE_EXT()       mp_keyboard_enable()
extern bool mp_keyboard_enable(void);
#define  UDI_HID_KBD_DISABLE_EXT()      mp_keyboard_disable()
extern void mp_keyboard_disable(void);
#define  UDI_HID_KBD_CHANGE_LED(value)  mp_keyboard_led(value)
extern void mp_keyboard_led(uint8_t);

//! Enable id string of interface to add an extra USB string
#define  UDI_HID_KBD_STRING_ID            7

/**
 * USB HID Keyboard low level configuration
 * In standalone these configurations are defined by the HID Keyboard module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_HID_KBD_EP_IN           (7 | USB_EP_DIR_IN)

//! Interface number
#define  UDI_HID_KBD_IFACE_NUMBER    4

/**
 * Description of Composite Device
 * @{
 */
//! USB Interfaces descriptor structure
#define UDI_COMPOSITE_DESC_T \
	usb_iad_desc_t       udi_cdc_iad; \
	udi_cdc_comm_desc_t  udi_cdc_comm; \
	udi_cdc_data_desc_t  udi_cdc_data; \
	udi_msc_desc_t       udi_msc; \
	udi_hid_mouse_desc_t udi_hid_mouse; \
	udi_hid_kbd_desc_t   udi_hid_kbd

//! USB Interfaces descriptor value for Full Speed
#define UDI_COMPOSITE_DESC_FS \
	.udi_cdc_iad   = UDI_CDC_IAD_DESC_0, \
	.udi_cdc_comm  = UDI_CDC_COMM_DESC_0, \
	.udi_cdc_data  = UDI_CDC_DATA_DESC_0_FS, \
	.udi_msc       = UDI_MSC_DESC_FS, \
	.udi_hid_mouse = UDI_HID_MOUSE_DESC, \
	.udi_hid_kbd   = UDI_HID_KBD_DESC

//! USB Interfaces descriptor value for High Speed
#define UDI_COMPOSITE_DESC_HS \
	.udi_cdc_iad   = UDI_CDC_IAD_DESC_0, \
	.udi_cdc_comm  = UDI_CDC_COMM_DESC_0, \
	.udi_cdc_data  = UDI_CDC_DATA_DESC_0_HS, \
	.udi_msc       = UDI_MSC_DESC_HS, \
	.udi_hid_mouse = UDI_HID_MOUSE_DESC, \
	.udi_hid_kbd   = UDI_HID_KBD_DESC

//! USB Interface APIs
#define UDI_COMPOSITE_API \
	&udi_api_cdc_comm, \
	&udi_api_cdc_data, \
	&udi_api_msc, \
	&udi_api_hid_mouse, \
	&udi_api_hid_kbd
//@}

/**
 * USB Device Driver Configuration
 * @{
 */
//@}

//! The includes of classes and other headers must be done at the end of this file to avoid compile error
#include "udi_cdc.h"
#include "udi_msc.h"
#include "udi_hid_mouse.h"
#include "udi_hid_kbd.h"

#endif
