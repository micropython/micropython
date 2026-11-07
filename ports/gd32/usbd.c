// usbd.c - USB device (TinyUSB DWC2) support for the GD32F4xx port.
//
// The GD32F4 USBFS controller is a Synopsys DWC2 core at 0x5000_0000 with four
// IN/OUT endpoints, the same IP TinyUSB already drives for STM32.  This file
// only brings the controller out of reset, routes the pins and forwards the
// interrupt; the class implementations come from shared/tinyusb.

#include "py/runtime.h"
#include "py/mphal.h"

#include "gd32f4xx.h"
#include "pin_defs.h"
#include "board.h"

#if MICROPY_HW_ENABLE_USBDEV

#include "tusb.h"
#include "shared/tinyusb/mp_usbd.h"

#define USB_GPIO_PORT   GPIOA
#define USB_GPIO_RCU    RCU_GPIOA
#define USB_DM_PIN      GPIO_PIN_11
#define USB_DP_PIN      GPIO_PIN_12
#define USB_AF          GPIO_AF_10

// ---- port hooks expected by shared/tinyusb ----
void mp_usbd_port_get_serial_number(char *serial_buf) {
    // Derive a stable serial from the 96-bit factory UID.
    const uint32_t *uid = (const uint32_t *)0x1FFF7A10;
    mp_uint_t len = 0;
    for (int i = 0; i < 3; i++) {
        static const char hex[] = "0123456789ABCDEF";
        for (int shift = 28; shift >= 0; shift -= 4) {
            serial_buf[len++] = hex[(uid[i] >> shift) & 0xF];
        }
    }
    serial_buf[len] = '\0';
}

static void usbd_pin_config(void) {
    rcu_periph_clock_enable(USB_GPIO_RCU);

    // USBFS uses AF10 on PA11 (DM) / PA12 (DP).
    gpio_af_set(USB_GPIO_PORT, USB_AF, USB_DM_PIN | USB_DP_PIN);
    gpio_mode_set(USB_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, USB_DM_PIN | USB_DP_PIN);
    gpio_output_options_set(USB_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                            USB_DM_PIN | USB_DP_PIN);
}

void usbd_init(void) {
    // Bring the USBFS controller out of reset and enable its clock.
    rcu_periph_clock_enable(RCU_USBFS);
    rcu_periph_reset_enable(RCU_USBFSRST);
    rcu_periph_reset_disable(RCU_USBFSRST);

    usbd_pin_config();
}

// The DWC2 IRQ is shared with the TinyUSB device stack.
void USBFS_IRQHandler(void) {
    tud_int_handler(0);
}

// mp_usbd_task() is driven from the main loop; nothing else is required here.
#endif // MICROPY_HW_ENABLE_USBDEV
