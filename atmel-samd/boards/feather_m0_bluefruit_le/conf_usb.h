
#include <stdbool.h>
#include <stdint.h>

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
#  define USB_DEVICE_MANUFACTURE_NAME "Arduino LLC"
#endif

#ifndef USB_DEVICE_PRODUCT_NAME
#  define USB_DEVICE_PRODUCT_NAME "Arduino Zero"
#endif
// #define  USB_DEVICE_SERIAL_NAME           "12...EF"

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
#define  UDI_CDC_SET_CODING_EXT(port,cfg)
#define  UDI_CDC_SET_DTR_EXT(port,set)
#define  UDI_CDC_SET_RTS_EXT(port,set)

#include "udi_cdc_conf.h" // At the end of conf_usb.h file
#endif
