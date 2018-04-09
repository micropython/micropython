/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "usb.h"

#include <stdint.h>

// We must include this early because it sets values used in the ASF4 includes
// below.
#include "py/mpconfig.h"

#include "hal/include/hal_gpio.h"
#include "usb/class/cdc/device/cdcdf_acm.h"
#include "usb/class/hid/device/hiddf_generic.h"
#include "usb/class/composite/device/composite_desc.h"
#include "usb/class/msc/device/mscdf.h"
#include "peripheral_clk_config.h"
#include "hpl/pm/hpl_pm_base.h"
#include "hpl/gclk/hpl_gclk_base.h"

#include "lib/utils/interrupt_char.h"
#include "genhdr/autogen_usb_descriptor.h"
#include "reset.h"
#include "usb_mass_storage.h"

#include "supervisor/shared/autoreload.h"

extern struct usbd_descriptors descriptor_bounds;

// Store received characters on our own so that we can filter control characters
// and act immediately on CTRL-C for example.

// Receive buffer
static uint8_t usb_rx_buf[USB_RX_BUF_SIZE];

// Receive buffer head
static volatile uint8_t usb_rx_buf_head = 0;

// Receive buffer tail
static volatile uint8_t usb_rx_buf_tail = 0;

// Number of bytes in receive buffer
volatile uint8_t usb_rx_count = 0;

volatile bool mp_cdc_enabled = false;
volatile bool usb_transmitting = false;

/** Ctrl endpoint buffer */
COMPILER_ALIGNED(4) static uint8_t ctrl_buffer[64];

static void init_hardware(void) {
    #ifdef SAMD21
    _pm_enable_bus_clock(PM_BUS_APBB, USB);
    _pm_enable_bus_clock(PM_BUS_AHB, USB);
    _gclk_enable_channel(USB_GCLK_ID, GCLK_CLKCTRL_GEN_GCLK0_Val);
    #endif

    #ifdef SAMD51
    hri_gclk_write_PCHCTRL_reg(GCLK, USB_GCLK_ID, CONF_GCLK_USB_SRC | GCLK_PCHCTRL_CHEN);
    hri_mclk_set_AHBMASK_USB_bit(MCLK);
    hri_mclk_set_APBBMASK_USB_bit(MCLK);
    #endif

    usb_d_init();

    gpio_set_pin_direction(PIN_PA24, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(PIN_PA24, false);
    gpio_set_pin_pull_mode(PIN_PA24, GPIO_PULL_OFF);
    gpio_set_pin_direction(PIN_PA25, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(PIN_PA25, false);
    gpio_set_pin_pull_mode(PIN_PA25, GPIO_PULL_OFF);
    #ifdef SAMD21
    gpio_set_pin_function(PIN_PA24, PINMUX_PA24G_USB_DM);
    gpio_set_pin_function(PIN_PA25, PINMUX_PA25G_USB_DP);
    #endif
    #ifdef SAMD51
    gpio_set_pin_function(PIN_PA24, PINMUX_PA24H_USB_DM);
    gpio_set_pin_function(PIN_PA25, PINMUX_PA25H_USB_DP);
    #endif
}

#define CDC_BULKOUT_SIZE CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKOUT_MAXPKSZ
COMPILER_ALIGNED(4) uint8_t cdc_packet_buffer[CDC_BULKOUT_SIZE];
static volatile bool pending_read;

static int32_t start_read(void) {
    pending_read = true;
    int32_t result = cdcdf_acm_read(cdc_packet_buffer, CDC_BULKOUT_SIZE);
    if (result != ERR_NONE) {
        pending_read = false;
    }
    return result;
}

static bool read_complete(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count) {
    if (rc != USB_XFER_DONE) {
        return false; // No errors.
    }
    pending_read = false;
    volatile hal_atomic_t flags;
    atomic_enter_critical(&flags);
    // If our buffer can't fit the data received, then error out.
    if (count > (uint8_t) (USB_RX_BUF_SIZE - usb_rx_count)) {
        atomic_leave_critical(&flags);
        return true;
    }

    for (uint16_t i = 0; i < count; i++) {
        uint8_t c = cdc_packet_buffer[i];
        if (c == mp_interrupt_char) {
            mp_keyboard_interrupt();
            // If interrupted, flush all the input.
            usb_rx_count = 0;
            usb_rx_buf_head = 0;
            usb_rx_buf_tail = 0;
            break;
        } else {
            // The count of characters present in receive buffer is
            // incremented.
            usb_rx_count++;
            usb_rx_buf[usb_rx_buf_tail] = c;
            usb_rx_buf_tail++;
            if (usb_rx_buf_tail == USB_RX_BUF_SIZE) {
                // Reached the end of buffer, revert back to beginning of
                // buffer.
                usb_rx_buf_tail = 0;
            }
        }
    }
    atomic_leave_critical(&flags);

    /* No error. */
    return false;
}

static bool write_complete(const uint8_t ep,
                           const enum usb_xfer_code rc,
                           const uint32_t count) {
    if (rc != USB_XFER_DONE) {
        return false; // No errors.
    }
    // This is called after writes are finished.

    usb_transmitting = false;

    /* No error. */
    return false;
}

volatile bool reset_on_disconnect = false;
volatile bool cdc_connected = false;

static bool usb_device_cb_state_c(usb_cdc_control_signal_t state)
{
    cdc_connected = state.rs232.DTR;
    if (state.rs232.DTR) {
    } else if (!state.rs232.DTR && reset_on_disconnect) {
        reset_to_bootloader();
    }

    /* No error. */
    return false;
}

static bool usb_device_cb_line_coding_c(const usb_cdc_line_coding_t* coding)
{
    reset_on_disconnect = coding->dwDTERate == 1200;
    /* Ok to change. */
    return true;
}

void init_usb(void) {
    init_hardware();

    mp_cdc_enabled = false;

    usbdc_init(ctrl_buffer);

    /* usbdc_register_function inside */
    cdcdf_acm_init();
    pending_read = false;

    mscdf_init(1);
    mscdf_register_callback(MSCDF_CB_INQUIRY_DISK, (FUNC_PTR)usb_msc_inquiry_info);
    mscdf_register_callback(MSCDF_CB_GET_DISK_CAPACITY, (FUNC_PTR)usb_msc_get_capacity);
    mscdf_register_callback(MSCDF_CB_START_READ_DISK, (FUNC_PTR)usb_msc_new_read);
    mscdf_register_callback(MSCDF_CB_START_WRITE_DISK, (FUNC_PTR)usb_msc_new_write);
    mscdf_register_callback(MSCDF_CB_EJECT_DISK, (FUNC_PTR)usb_msc_disk_eject);
    mscdf_register_callback(MSCDF_CB_TEST_DISK_READY, (FUNC_PTR)usb_msc_disk_is_ready);
    mscdf_register_callback(MSCDF_CB_XFER_BLOCKS_DONE, (FUNC_PTR)usb_msc_xfer_done);
    mscdf_register_callback(MSCDF_CB_IS_WRITABLE, (FUNC_PTR)usb_msc_disk_is_writable);

    hiddf_generic_init(hid_report_descriptor, sizeof(hid_report_descriptor));

    usbdc_start(&descriptor_bounds);

    usbdc_attach();


}

static bool cdc_enabled(void) {
    if (!cdcdf_acm_is_enabled()) {
        mp_cdc_enabled = false;
        return false;
    }
    if (!mp_cdc_enabled) {
        cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)read_complete);
        cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)write_complete);
        cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)usb_device_cb_state_c);
        cdcdf_acm_register_callback(CDCDF_ACM_CB_LINE_CODING_C, (FUNC_PTR)usb_device_cb_line_coding_c);
        mp_cdc_enabled = true;
    }

    return true;
}

bool usb_bytes_available(void) {
    // Check if the buffer has data, but not enough
    // space to hold another read.
    if (usb_rx_count > USB_RX_BUF_SIZE - CDC_BULKOUT_SIZE) {
        return true;
    }
    // Buffer has enough room
    if (cdc_enabled() && !pending_read) {
        start_read();
    }
    // Buffer is empty and/or no new data is available
    if (usb_rx_count == 0) {
        return false;
    }
    return usb_rx_count > 0;
}

int usb_read(void) {
    if (!cdc_enabled() || usb_rx_count == 0) {
        return 0;
    }

    // Copy from head.
    int data;
    CRITICAL_SECTION_ENTER();
    data = usb_rx_buf[usb_rx_buf_head];
    usb_rx_buf_head++;
    usb_rx_count--;
    if (usb_rx_buf_head == USB_RX_BUF_SIZE) {
      usb_rx_buf_head = 0;
    }
    CRITICAL_SECTION_LEAVE();

    return data;
}

// TODO(tannewt): See if we can disable the internal CDC IN cache since we
// we manage this one ourselves.
#define CDC_BULKIN_SIZE CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKIN_MAXPKSZ
COMPILER_ALIGNED(4) uint8_t cdc_output_buffer[CDC_BULKIN_SIZE];

void usb_write(const char* buffer, uint32_t len) {
    if (!cdc_enabled()) {
        return;
    }
    if (!cdc_connected) {
        // TODO(tannewt): Should we write to a file instead?
        return;
    }
    uint8_t * output_buffer;
    uint8_t output_len;
    while (len > 0) {
        while (usb_transmitting) {}
        output_buffer = (uint8_t *) buffer;
        output_len = len;
        // Use our own cache in two different cases:
        //   * When we're at the end of a transmission and we'll return before
        //     the given buffer is actually transferred to the USB device.
        //   * When our given buffer isn't aligned on word boundaries.
        if (output_len <= CDC_BULKIN_SIZE || ((uint32_t) buffer) % 4 != 0) {
            output_buffer = cdc_output_buffer;
            output_len = output_len > CDC_BULKIN_SIZE ? CDC_BULKIN_SIZE : output_len;
            memcpy(cdc_output_buffer, buffer, output_len);
        } else {
            output_len = CDC_BULKIN_SIZE;
        }
        usb_transmitting = true;
        cdcdf_acm_write(output_buffer, output_len);
        buffer += output_len * sizeof(char);
        len -= output_len;
    }
}

bool usb_connected(void) {
    return cdc_enabled();
}

// Poll for input if keyboard interrupts are enabled,
// so that we can check for the interrupt char. read_complete() does the checking.
// also make sure we have enough room in the local buffer
void usb_cdc_background() {
    //
    if (mp_interrupt_char != -1 && cdc_enabled() && !pending_read && (usb_rx_count < USB_RX_BUF_SIZE - CDC_BULKOUT_SIZE)) {
        start_read();
    }
}
