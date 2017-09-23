/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#include "atmel_start.h"
#include "usb_start.h"

static uint8_t multi_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    COMPOSITE_DESCES_LS_FS,
};

static struct usbd_descriptors multi_desc = {multi_desc_bytes, multi_desc_bytes + sizeof(multi_desc_bytes)};

/** Ctrl endpoint buffer */
static uint8_t ctrl_buffer[64];

/**
 * \brief Composite Init
 */
void composite_device_init(void)
{
	/* usb stack init */
	usbdc_init(ctrl_buffer);

	/* usbdc_register_funcion inside */
	cdcdf_acm_init();

	usbdc_start(&multi_desc);
	usbdc_attach();
}

/**
 * Example of using Composite Function.
 * \note
 * In this example, we will use a PC as a USB host:
 * - Connect the DEBUG USB on XPLAINED board to PC for program download.
 * - Connect the TARGET USB on XPLAINED board to PC for running program.
 */
void composite_device_example(void)
{
	while (!cdcdf_acm_is_enabled()) {
		// wait cdc acm to be installed
	};
}

void usb_init(void)
{

	composite_device_init();
}
