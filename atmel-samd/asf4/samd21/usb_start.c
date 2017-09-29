/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#include "atmel_start.h"
#include "usb_start.h"

#define CONF_USB_MSC_MAX_LUN 0

static uint8_t multi_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    COMPOSITE_DESCES_LS_FS,
};

static struct usbd_descriptors multi_desc = {multi_desc_bytes, multi_desc_bytes + sizeof(multi_desc_bytes)};

/** Ctrl endpoint buffer */
static uint8_t ctrl_buffer[64];

void composite_device_init(void)
{
	/* usb stack init */
	usbdc_init(ctrl_buffer);

/* usbdc_register_funcion inside */
#if CONF_USB_COMPOSITE_CDC_ACM_EN
	cdcdf_acm_init();
#endif
#if CONF_USB_COMPOSITE_HID_MOUSE_EN
	hiddf_mouse_init();
#endif
#if CONF_USB_COMPOSITE_HID_KEYBOARD_EN
	hiddf_keyboard_init();
#endif
#if CONF_USB_COMPOSITE_MSC_EN
	mscdf_init(CONF_USB_MSC_MAX_LUN);
#endif
}

void composite_device_start(void)
{
	usbdc_start(&multi_desc);
	usbdc_attach();
}

void composite_device_example(void)
{

	/* Initialize */
	/* It's done with system init ... */

	/* Before start do function related initializations */
	/* Add your code here ... */

	/* Start device */
	composite_device_start();

	/* Main loop */
	while (1) {
		if (cdcdf_acm_is_enabled()) {
			/* CDC ACM process*/
		}
		if (hiddf_mouse_is_enabled()) {
			/* HID Mouse process */
		}
		if (hiddf_keyboard_is_enabled()) {
			/* HID Keyboard process */
		};
		if (mscdf_is_enabled()) {
			/* MSC process */
		}
	}
}

void usb_init(void)
{

	composite_device_init();
}
