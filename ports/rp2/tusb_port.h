#ifndef TUSB_PORT_H_
#define TUSB_PORT_H_

enum {
	VENDOR_REQUEST_WEBUSB = 1,
	VENDOR_REQUEST_MICROSOFT = 2,
	VENDOR_REQUEST_WEBUSB_CONNECTION = 34
};

extern const uint8_t desc_ms_os_20[];

#endif /* TUSB_PORT_H_ */
